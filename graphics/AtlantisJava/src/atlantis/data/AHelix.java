package atlantis.data;

import atlantis.globals.AGlobals;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjectionVP;
import atlantis.utils.A3Vector;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;

/**
 * Base class for tracks with perigee parameters. Includes code for drawing
 * helices as a function of the helix angle alpha.
 *
 * @author Eric Jansen
 */
public class AHelix {

    private final static ALogger logger = ALogger.getLogger(AHelix.class);
    
    // Constants
    public static final double MIN_RADIUS_PHI = 2.;
    public static final double TRACKER_RADIUS = 108.;
    public static final double TRACKER_LENGTH = 270.;
    public static final double SOLENOID_RADIUS = 125.;
    public static final double SOLENOID_LENGTH = 280.;
    protected static final int NUM_HELIX_POINTS = 30;

    // Parameters from the config/GUI we need
    private static double curvature = -1;
    
    // Vertex w.r.t. which we define the perigee parameters
    private float xVtx, yVtx, zVtx;

    // Perigee parameters for the track
    private float d0, phi0, z0, cotanTheta, pt;
    
    // Additional space points outside the solenoid (muon tracks)
    private float extraX[] = null;
    private float extraY[] = null;
    private float extraZ[] = null;
    
    // Covariance matrix
    private double[][] cov = null;

    // Center point, radius and correction factors for the helix
    private double xC, yC, R, dr1, dr2, dz1;

    // Sign of alpha, determines direction of curvature of the helix
    private float S;

    // Alpha value where the helix crosses MIN_RADIUS_PHI. This is the starting
    // value for all the phi-based projections, since phi is undefined at rho=0.
    private double alphaMin = 0.;

    // Alpha value at which the helix meets its last space point or where it
    // exits the tracker if no space points were defined.
    private double alphaMax = Math.PI;

    // Alpha value at which the helix leaves the solenoidal field. When helices
    // are drawn beyond this value, for example when extrapolating to the
    // calorimeters, they switch from a helix to a straight line here.
    private double alphaSolenoid = Math.PI;

    private static final APar parameterStore = APar.instance();

    AHelix(AOldHelix helix) {
        this((float)helix.d0, (float)helix.z0, (float)helix.phi0, (float)helix.tL, (float)helix.pT);
    }
    
    AHelix(float rhoVertex, float phiVertex, float zVertex,
            float pTTrack, float phiTrack, float etaTrack, int charge, float rhoEndVertex) {

        this(rhoVertex, phiVertex, zVertex, pTTrack, phiTrack, etaTrack, charge);
        this.alphaMax = getAlphaExtrapolated(rhoEndVertex, TRACKER_LENGTH);
    }
    
    AHelix(float rhoVertex, float phiVertex, float zVertex,
            float pTTrack, float phiTrack, float eta, int charge) {

        this(0.f, 0.f, phiTrack, (float)AMath.tanLambda(eta), charge*Math.abs(pTTrack),
                (float)(rhoVertex*Math.cos(phiVertex)), (float)(rhoVertex*Math.sin(phiVertex)), zVertex);
    }
    
    AHelix(float d0, float z0, float phi0, float tL, float pT, float[][] cov) {
        this(d0, z0, phi0, tL, pT);
        this.setCovariance(cov);
    }

    AHelix(float d0, float z0, float phi0, float cotanTheta, float pt) {
        this(d0, z0, phi0, cotanTheta, pt, 0.f, 0.f, 0.f);
    }

    /**
     * Construct a new track from its perigee parameters.
     * @param d0 transverse impact parameter
     * @param z0 longitudinal impact parameter
     * @param phi0 phi of the point of closest approach in degrees
     * @param cotanTheta cotan(theta), angle of the track in the RZ-plane
     * @param pt signed transverse momentum, sign encodes charge
     * @param xVtx vertex x-coordinate
     * @param yVtx vertex y-coordinate
     * @param zVtx vertex z-coordinate
     */
    AHelix(float d0, float z0, float phi0, float cotanTheta, float pt,
            float xVtx, float yVtx, float zVtx) {
        
        this.xVtx = xVtx;
        this.yVtx = yVtx;
        this.zVtx = zVtx;
        this.d0 = d0;
        this.z0 = z0;
        this.phi0 = (float)Math.toRadians(phi0);
        this.cotanTheta = cotanTheta; // cotan(theta) = tan(lambda)
        this.pt = Math.abs(pt);
        this.S = -Math.signum(pt);
        this.dr1 = 0;
        this.dr2 = 0;
        this.dz1 = 0;

        // Curvature/pT relation, magnetic field strength.
        if (AHelix.curvature <= 0) {
            AParameter curvatureParameter = parameterStore.get("Event", "Curvature");
            if (curvatureParameter != null) {
                AHelix.curvature = curvatureParameter.getD();
            } else {
                AHelix.curvature = 100/0.6;
                logger.error("Curvature parameter not found,"
                        + " defaulting to " + AHelix.curvature);
            }
        }

        // Radius and center point of the helix. The radius is limited to 1e6, 
        // which on the scale of our tracker corresponds to a straight line. 
        // This allows cosmics without B-field to be drawn with the same code.
        this.R = Math.min(AHelix.curvature * this.pt, 1e6);
        this.xC = xVtx - (this.S*this.R + this.d0) * Math.sin(this.phi0);
        this.yC = yVtx + (this.S*this.R + this.d0) * Math.cos(this.phi0);

        // The alpha value from which the helix start.
        this.alphaMin = 0.;
        
        // Calculate when this helix leaves the tracker. This is either (1)
        // through the barrel or (2) through the endcap. The endcap expression
        // is just the function getZ(alpha) reversed. Take the minumum of the
        // two, so whatever happens first.
        this.alphaMax = getAlphaCylinder(TRACKER_RADIUS, TRACKER_LENGTH);

        // Same as above, but now for the solenoid.
        this.alphaSolenoid = getAlphaCylinder(SOLENOID_RADIUS, SOLENOID_LENGTH);
    }
    
    /**
     * Fit the radius of the helix to the given points. The radius is described
     * by R + dr1 * alpha + dr2 * alpha^2.
     * @param x array of x coordinates of points on the helix
     * @param y array of y coordinates of points on the helix
     * @param z array of z coordinates of points on the helix
     */
    public double setPoints(float[] x, float[] y, float[] z, int numPoints) {

        this.dr1 = 0.;
        this.dr2 = 0.;
        this.dz1 = 0.;
        
        // Points outside the solenoid are taken as is and added to the end of the track
        while (numPoints > 0 && (z[numPoints-1] > SOLENOID_LENGTH
                || Math.hypot(x[numPoints-1], y[numPoints-1]) > SOLENOID_RADIUS)) {

            numPoints--;
        }
        
        if (x.length > numPoints) {
            extraX = new float[x.length - numPoints];
            extraY = new float[y.length - numPoints];
            extraZ = new float[z.length - numPoints];
            
            for(int i=0; i<x.length - numPoints; i++) {
                extraX[i] = x[numPoints+i];
                extraY[i] = y[numPoints+i];
                extraZ[i] = z[numPoints+i];
            }
        }
        
        if (numPoints == 1) {

            // Just one point, only do a linear in/decrease of R and Z
            double alpha = getAlpha(x[0], y[0]);
            this.dr1 = (getRadius(x[0], y[0]) - this.R) / alpha;
            this.dz1 = (z[0] - getZ(alpha)) / alpha;

            // Draw helix until this point
            this.alphaMax = alpha;

        } else {
            
            // Several points, perform a least-squares fit using the functions
            // R = R + dr1 * alpha + dr2 * alpha^2 and Z = Z + dz1 * alpha.
            double a2 = 0., a3 = 0., a4 = 0.;
            double dRa = 0., dRa2 = 0., dZa = 0.;
            for (int i=0; i<numPoints; i++) {
                double alpha = getAlpha(x[i], y[i]);
                double rho = getRadius(x[i], y[i]);

                //a1 += alpha;
                a2 += Math.pow(alpha, 2.);
                a3 += Math.pow(alpha, 3.);
                a4 += Math.pow(alpha, 4.);
                dRa += (rho - this.R) * alpha;
                dRa2 += (rho - this.R) * alpha * alpha;
                
                dZa += (z[i] - getZ(alpha)) * alpha;

                if (i == 0 || alpha > this.alphaMax) {
                    this.alphaMax = alpha;
                }
            }

            // Quadratic
            this.dr1 = (dRa / a3 - dRa2 / a4) / (a2 / a3 - a3 / a4);
            this.dr2 = (dRa / a2 - dRa2 / a3) / (a3 / a2 - a4 / a3);

            // Linear
            this.dz1 = dZa / a2;

        }

        // Return relative correction; distance from the original position at
        // the end point, divided by the length of the curve.
        double change = Math.hypot(this.dr1 + this.alphaMax*this.dr2, this.dz1) / this.R;
        logger.debug("Fitting changed R(maxAlpha) by " + change);
        return change;
    }

    /**
     * Sets the covariance matrix of the track.
     * @param cov new covariance matrix
     */
    public final void setCovariance(float[][] cov) {
        
        // Covariance needs to be double[][] for the fitting classes
        this.cov = new double[cov.length][cov[0].length];
        
        for (int i=0; i<cov.length; i++) {
            for (int j=0; j<cov[i].length; j++) {
                this.cov[i][j] = cov[i][j];
            }
        }
    }

    /**
     * Sets the covariance matrix of the track.
     * @param cov new covariance matrix
     */
    public final void setCovariance(double[][] cov) {
        this.cov = cov;
    }
    
    /**
     * Returns the starting alpha value for the phi-based projections.
     * @return alpha value
     */
    public double getAlphaMin() {
        return alphaMin;
    }

    /**
     * Returns the alpha value at the end of the helix
     * @return alpha value
     */
    public double getAlphaMax() {
        return alphaMax;
    }

    /**
     * Returns the alpha value at which the track exits the solenoidal field
     * @return alpha value
     */
    public double getAlphaSolenoid() {
        return alphaSolenoid;
    }

    /**
     * Calculates the alpha coordinate for a specific point. Alpha is the angle
     * from the center point of the helix to the point. For the helix The angle
     * should be positive and alpha=0 is the perigee. Returns negative values
     * for points before the perigee point.
     * @param x x coordinate of a point
     * @param y y coordinate of a point
     * @return alpha
     */
    public double getAlpha(double x, double y) {
        // Get the alpha parameter for this point, in the range 0-2*pi
        double alpha = AMath.nearestPhiRadians(this.S * (Math.atan2((y - this.yC), (x - this.xC)) - this.phi0) + Math.PI/2.);

        // When dealing with a real space point, alpha can of course be slightly negative
        if (alpha > 3./2. * Math.PI) alpha -= 2.*Math.PI;

        return alpha;
    }

    /**
     * Calculates the alpha coordinate where the helix has the given distance
     * from the origin. Alpha is the angle from the center point of the helix to
     * the point. The angle is always positive and alpha=0 is the perigee.
     * @param rho radius with respect to the origin
     * @return alpha
     */
    public final double getAlphaCylinder(double rho, double z) {
        
        double alphaZ, alphaRho;
        if (this.cotanTheta == 0) {
            alphaZ = Math.PI;
        } else {
            alphaZ = zVtx + (Math.signum(this.cotanTheta)*z - this.z0) / (this.cotanTheta * this.R + this.dz1);
        }

        // We take a circle centered around (0,0) with the requested radius
        // (rho) and intersect it with the helix (circle around (xC,yC) with
        // radius R. The distance between the centers of the two circles is D.
        double D = Math.hypot(this.xC, this.yC);

        // By equating the intersection points in both circles we obtain the
        // position of the intersection points as a distance d from (xC,yC) 
        // along the line from center to center.
        double d = (this.R * this.R - rho * rho + D * D) / (2 * D);

        if (d >= this.R) {
            // Helix is fully outside the requested radius, return 0.
            alphaRho = 0;
        } else if (d <= -this.R) {
            // Helix is fully inside the requested radius, return pi.
            alphaRho = Math.PI;
        } else {
            // Helix needs to be stopped when it reaches requested radius.
            // Now the angle alpha is just the angle between the center-center
            // line and the line from (xC,yC) to the intersection point. The
            // helix is such that alpha is always taken positive, so it doesn't
            // matter which of the two intersection points we take. The cosine
            // of alpha is then simply the division of d by the radius of the
            // circle R. This method should never return an alpha outside the 
            // solenoid, for the extrapolation there is getAlphaExtrapolated().
            alphaRho = Math.min(Math.acos(d / this.R), alphaSolenoid);
        }

        return Math.min(alphaRho, alphaZ);
    }

    /**
     * Calculates the pseudo alpha coordinate and if necessary extrapolates 
     * beyond the curved part up to the given radius. Inside the solenoid this 
     * method falls back to getAlphaCylinder, so it can be used for any radius.
     * The pseudo alpha coordinate is such that the distance along the curve is 
     * R*alpha also for the straight part. This parameterization allow the 
     * description of z to remain the same for any alpha.
     * @param rho radius to extrapolate to
     * @param z distance along the beam axis to extrapolate to
     * @return alpha
     */
    public final double getAlphaExtrapolated(double rho, double z) {

        // Check if the track actually makes it to the solenoid.
        if (this.alphaSolenoid >= Math.PI) {
            // If not, return the max alpha inside the tracker instead.
            return getAlphaCylinder(rho, z);
        }

        // Calculate the coordinates (xL,yL) of the last point inside the solenoid.
        double xL = getX(this.alphaSolenoid);
        double yL = getY(this.alphaSolenoid);

        // Check if we are actually outside the solenoid at this rho value
        if (rho < Math.hypot(xL, yL)) {
            // If not, return the normal alpha instead. For tracks that exit the
            // solenoid through the endcap this is also fine, z(alpha) is the
            // same inside and outside the field. So the max value returned is
            // valid either way.
            return getAlphaCylinder(rho, z);
        }

        double alphaZ, alphaRho;
        if (this.cotanTheta == 0) {
            alphaZ = Math.PI;
        } else {
            alphaZ = zVtx + (Math.signum(this.cotanTheta)*z - this.z0) / (this.cotanTheta * this.R + this.dz1);
        }

        // Calculate the direction of the track at the last point inside the solenoid,
        // this is the direction in which it will be extrapolated through the calorimeters.
        double dx = Math.cos(this.phi0 + this.S * this.alphaSolenoid);
        double dy = Math.sin(this.phi0 + this.S * this.alphaSolenoid);

        // Now we have for a point on the extrapolated track
        //   x(alpha) = xL + dx * (alpha-this.alphaCurve)
        //   y(alpha) = yL + dy * (alpha-this.alphaCurve)
        // Setting x^2+y^2 = rho^2 and solving for alpha then gives:
        double D = rho*rho - (xL*dy - yL*dx)*(xL*dy - yL*dx);
        if (D > 0) {
            alphaRho = this.alphaSolenoid - (xL*dx + yL*dy)/this.R + Math.sqrt(D)/this.R;
        } else {
            return Math.PI;
        }

        return Math.min(alphaRho, alphaZ);
    }

    /**
     * Calculate the distance between a point and the center of the helix.
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @return distance between (x,y) and the center of the helix
     */
    private double getRadius(double x, double y) {
        return Math.hypot(x - xC, y - yC);
    }

    /**
     * Returns the x coordinate for a point on the helix parameterized by alpha.
     * @param alpha helix angular parameter
     * @return x coordinate
     */
     public double getX(double alpha) {
        // Correct the radius using the values obtained from the fit and calculate x
        double localR = R + dr1 * alpha + dr2 * alpha * alpha;
        if (alpha <= alphaSolenoid) {
            return xC - S * localR * Math.cos(phi0 + S * alpha + Math.PI/2.);
        } else {
            double xL = xC - S * localR * Math.cos(phi0 + S * alphaSolenoid + Math.PI/2.);
            return xL + R*Math.cos(phi0 + this.S * alphaSolenoid) * (alpha-alphaSolenoid);
        }
    }

    /**
     * Returns the y coordinate for a point on the helix parameterized by alpha.
     * @param alpha helix angular parameter
     * @return y coordinate
     */
    public double getY(double alpha) {
        // Correct the radius using the values obtained from the fit and calculate y
        double localR = R + dr1 * alpha + dr2 * alpha * alpha;
        if (alpha <= alphaSolenoid) {
            return yC - S * localR * Math.sin(phi0 + S * alpha + Math.PI/2.);
        } else {
            double yL = yC - this.S * localR * Math.sin(phi0 + this.S * alphaSolenoid + Math.PI/2.);
            return yL + R*Math.sin(phi0 + S * alphaSolenoid) * (alpha-alphaSolenoid);
        }
    }

    /**
     * Returns the rho coordinate for a point on the helix parameterized by alpha.
     * @param alpha helix angular parameter
     * @return rho coordinate
     */
    public double getRho(double alpha) {
        return Math.hypot(getX(alpha), getY(alpha));
    }

    public double getRhoVtx() {
        return Math.hypot(xVtx, yVtx);
    }
    
    /**
     * Returns the z coordinate for a point on the helix parameterized by alpha.
     * @param alpha helix angular parameter
     * @return z coordinate
     */
    public double getZ(double alpha) {
        // For z we don't use the corrected R, since it was calculated against
        // the uncorrected one.
        return z0 + cotanTheta * R * alpha + dz1 * alpha;
    }

    public double getZVtx() {
        return zVtx;
    }
        
    /**
     * Returns the phi coordinate of a point on the helix parameterized by alpha.
     * @param alpha helix angular parameter
     * @param useVertex calculate phi w.r.t. the primary vertex
     * @return phi coordinate
     */
    public double getPhi(double alpha, boolean useVertex) {
        if (useVertex) {
            return AParameterUtilities.phi(getX(alpha), getY(alpha));
        } else {
            return Math.toDegrees(Math.atan2(getY(alpha), getX(alpha)));
        }
    }

    /**
     * Return eta coordinate for a point on the helix parameterized by alpha.
     * @param alpha helix angular parameter
     * @param vsign side (-1/+1) of the split track in the v-plot, 0 for true eta
     * @return eta coordinate
     */
    public double getEta(double alpha, int vsign) {
        double rho = getRho(alpha);
        double z = getZ(alpha);
        double deta = Math.abs(AProjectionVP.getDeltaEta(rho, z));

        return AParameterUtilities.eta(z, rho) + vsign*deta;
    }
    
    /**
     * Returns an array representing the x coordinates of the helix between
     * alpha1 and alpha2.
     * @param alpha1 start alpha
     * @param alpha2 end alpha
     * @return x coordinates
     */
    public double[] getX(double alpha1, double alpha2) {
        int numPoints = NUM_HELIX_POINTS;
        if (alpha2 >= alphaMax && extraX != null) {
            numPoints += extraX.length;
        }
        double[] x = new double[numPoints];

        double dalpha = (alpha2 - alpha1) / (NUM_HELIX_POINTS-1);
        for (int i=0; i<NUM_HELIX_POINTS; i++) {
            x[i] = getX(alpha1 + i*dalpha);
        }
        
        for (int i=0; i<numPoints-NUM_HELIX_POINTS; i++) {
            x[NUM_HELIX_POINTS+i] = extraX[i];
        }

        return x;
    }

    /**
     * Returns an array representing the y coordinates of the helix between
     * alpha1 and alpha2.
     * @param alpha1 start alpha
     * @param alpha2 end alpha
     * @return y coordinates
     */
    public double[] getY(double alpha1, double alpha2) {
        int numPoints = NUM_HELIX_POINTS;
        if (alpha2 >= alphaMax && extraY != null) {
            numPoints += extraY.length;
        }
        double[] y = new double[numPoints];

        double dalpha = (alpha2 - alpha1) / (NUM_HELIX_POINTS-1);
        for (int i=0; i<NUM_HELIX_POINTS; i++) {
            y[i] = getY(alpha1 + i*dalpha);
        }

        for (int i=0; i<numPoints-NUM_HELIX_POINTS; i++) {
            y[NUM_HELIX_POINTS+i] = extraY[i];
        }
        
        return y;
    }

    /**
     * Returns an array representing the z coordinates of the helix between
     * alpha1 and alpha2.
     * @param alpha1 start alpha
     * @param alpha2 end alpha
     * @return z coordinates
     */
    public double[] getZ(double alpha1, double alpha2) {
        int numPoints = NUM_HELIX_POINTS;
        if (alpha2 >= alphaMax && extraZ != null) {
            numPoints += extraZ.length;
        }
        double[] z = new double[numPoints];

        double dalpha = (alpha2 - alpha1) / (NUM_HELIX_POINTS-1);
        for (int i=0; i<NUM_HELIX_POINTS; i++) {
            z[i] = getZ(alpha1 + i*dalpha);
        }

        for (int i=0; i<numPoints-NUM_HELIX_POINTS; i++) {
            z[NUM_HELIX_POINTS+i] = extraZ[i];
        }
        return z;
    }
    
    /**
     * Returns an array representing the rho coordinates of the helix between
     * alpha1 and alpha2.
     * @param alpha1 start alpha
     * @param alpha2 end alpha
     * @param signed return signed rho values for use in RZ
     * @return rho coordinates
     */
    public double[] getRho(double alpha1, double alpha2, boolean signed) {
        int numPoints = NUM_HELIX_POINTS;
        if (alpha2 >= alphaMax && extraX != null && extraY != null) {
            numPoints += extraX.length;
        }
        double[] rho = new double[numPoints];

        double sign = 1.;
        if (signed) {
            AParameter phiPar = parameterStore.get("RZ", "Phi");
            if (phiPar != null) {
                double phi = getPhi((alpha1+alpha2)/2, false);
                double diff = Math.toRadians(Math.abs(phi - phiPar.getD()));
                if (diff > Math.PI / 2. && diff <= 3 * Math.PI / 2.) {
                    sign = -1.;
                }
            } else {
                logger.error("Could not read RZ cutting plane parameter");
            }
        }
                
        double dalpha = (alpha2 - alpha1) / (NUM_HELIX_POINTS-1);
        for (int i=0; i<NUM_HELIX_POINTS; i++) {
            rho[i] = sign * getRho(alpha1 + i*dalpha);
        }

        for (int i=0; i<numPoints-NUM_HELIX_POINTS; i++) {
            rho[NUM_HELIX_POINTS+i] = sign * Math.hypot(extraX[i], extraY[i]);
        }

        return rho;
    }

    /**
     * Returns an array representing the phi coordinates of the helix between
     * alpha1 and alpha2.
     * @param alpha1 start alpha
     * @param alpha2 end alpha
     * @param vplot return all points twice for use in the v-plot
     * @return phi coordinates
     */
    public double[] getPhi(double alpha1, double alpha2, boolean vplot) {
        int numPoints = NUM_HELIX_POINTS;
        if (vplot) {
            numPoints += NUM_HELIX_POINTS-1;
        } else if (alpha2 >= alphaMax && extraX != null && extraY != null) {
            numPoints += extraX.length;
        }
        double[] phi = new double[numPoints];

        double dalpha = (alpha2 - alpha1) / (NUM_HELIX_POINTS-1);
        for (int i=0; i<NUM_HELIX_POINTS; i++) {
            phi[i] = getPhi(alpha1 + i*dalpha, true);
            if (vplot) phi[numPoints-i-1] = phi[i];
        }

        if (!vplot) {
            for (int i=0; i<numPoints-NUM_HELIX_POINTS; i++) {
                phi[NUM_HELIX_POINTS+i] = AParameterUtilities.phi(extraX[i], extraY[i]);
            }
        }

        // Minimise distance from one point to the next to avoid phi wrapping 
        // around: use {25, 10, -5} and not {25, 10, 355}.
        for (int i=1; i<phi.length; i++) {
            phi[i] = AMath.nearestPhiDegrees(phi[i], phi[i-1]);
        }
        
        return phi;
    }

    /**
     * Returns an array representing the eta coordinates of the helix between
     * alpha1 and alpha2.
     * @param alpha1 start alpha
     * @param alpha2 end alpha
     * @return eta coordinates
     */
    public double[] getEta(double alpha1, double alpha2) {
        double[] eta = new double[2*NUM_HELIX_POINTS - 1];
        double dalpha = (alpha2 - alpha1) / (NUM_HELIX_POINTS-1);

        for (int i=0; i<NUM_HELIX_POINTS; i++) {
            double alpha = alpha1 + i*dalpha;
            eta[i] = getEta(alpha, -1);
            eta[2*NUM_HELIX_POINTS-2-i] = getEta(alpha, +1);
        }

        return eta;
    }
    
    /**
     * Track momentum at perigee, x component
     * @return px
     */
    public double pX() {
        return this.pt * Math.cos(this.phi0);
    }

    /**
     * Track momentum at perigee, y component
     * @return py
     */
    public double pY() {
        return this.pt * Math.sin(this.phi0);
    }

    /**
     * Track momentum at perigee, z component
     * @return pz
     */
    public double pZ() {
        return this.pt * this.cotanTheta;
    }

    /**
     * Signed transverse momentum at perigee
     * @return pt
     */
    public double pT() {
        return -this.S * this.pt;
    }

    public A3Vector p() {
        return new A3Vector(pX(), pY(), pZ());
    }
    
    /**
     * Pseudorapidity of track from perigee
     * @return eta
     */
    public double eta() {
        return -Math.log(Math.hypot(cotanTheta, 1.) - cotanTheta);
    }

    /**
     * Phi of perigee
     * @return phi0
     */
    public double phi0() {
        return (float)Math.toDegrees(phi0);
    }

    /**
     * Returns the parameters of this helix in the "old" format. This is still
     * used by the AFit class to fit secondary vertices in Atlantis.
     * @return helix parameters
     */
    public double[] getPar() { // Used by unit tests and AVertexFit
        return new double[] {
            d0, 
            z0, 
            phi0 < 0 ? phi0 + 2*Math.PI : phi0, 
            cotanTheta,
            -S/pt
        };
    }

    /**
     * Returns the covariance matrix of this helix as a an array of double.
     * @return covariance matrix
     */
    public double[][] getCovariance() {
        return cov;
    }
    
    public double getRhoEndVertex() {
        return getRho(alphaMax);
    }
    
    public void setPhiStartByXYPoint(double Vx, double Vy) {
        alphaMin = this.getAlpha(Vx, Vy);
    }
    
    public float d0() {
        return d0;
    }
    
    public float z0() {
        return z0;
    }
    
    public float cotanTheta() {
        return cotanTheta;
    }
        
    public double getXc() {
        return xC;
    }

    public double getYc() {
        return yC;
    }

    /**
     * Helper method for toString, formats a single line of information.
     * @param name variable name
     * @param value variable value
     * @param uncertainty variable uncertainty
     * @param units variable units
     * @return formatted string
     */
    private String formatInfo(String name, double value, double uncertainty, String units) {
        return formatInfo(name, String.format("%.3f", value), 
                uncertainty > 0 ? String.format("%.3f", uncertainty) : null, units);
    }
    
    /**
     * Helper method for toString, formats a single line of information.
     * @param name variable name
     * @param value variable value
     * @param uncertainty variable uncertainty
     * @param units variable units
     * @return formatted string
     */
    private String formatInfo(String name, String value, String uncertainty, String units) {
        String output = "\n ";
        output += name + " = " + value;
        if (uncertainty != null) 
            output += " " + AMath.PLUSMINUS + " " + uncertainty;
        if (units != null)
            output += " " + units;
        
        return output;
    }
    
    @Override
    public String toString() {
        String output = "";

        int simpleOutput = AGlobals.instance().getSimpleOutput();
        if (simpleOutput > 0) {
            if (simpleOutput == 1 || simpleOutput == 3) {
                output += formatInfo("PT", pt, 0., "GeV");
                output += formatInfo(AMath.ETA, eta(), 0., null);
                output += formatInfo(AMath.PHI, phi0(), 0., AMath.DEGREES);
            }
            if (simpleOutput == 2 || simpleOutput == 3) {
                output += formatInfo("Px", pt*Math.cos(phi0), 0., "GeV");
                output += formatInfo("Py", pt*Math.sin(phi0), 0., "GeV");
                output += formatInfo("Pz", pt*Math.sinh(eta()), 0., "GeV");
                output += formatInfo("Charge", S < 0 ? "+1" : "-1", null, null);
            }

            return output;

        } else {

            output += formatInfo("d0", d0, cov != null ? Math.sqrt(cov[0][0]) : 0., "cm");
            output += formatInfo("z0", z0, cov != null ? Math.sqrt(cov[1][1]) : 0., "cm");

            double pv[] = AParameterUtilities.getPrimaryVertex();
            output += formatInfo("|z0-zVtx|", Math.abs(z0-pv[2]), 0., "cm");
            output += formatInfo("phi0", phi0(), cov != null ? Math.toDegrees(Math.sqrt(cov[2][2])) : 0., AMath.DEGREES);
            output += formatInfo("    ", phi0, cov != null ? Math.sqrt(cov[2][2]) : 0., "rad");
            output += formatInfo("cotan(theta)", cotanTheta, cov != null ? Math.sqrt(cov[3][3]) : 0., null);
            output += formatInfo(AMath.ETA, eta(), cov != null ? Math.sqrt(cov[3][3] / (cotanTheta * cotanTheta + 1.0)) : 0., null);
            output += formatInfo("pT", pT(), 0., "GeV");
            output += formatInfo("p", AMath.getPFromPttL(pt, cotanTheta), 0., "GeV");

            return output;
        }
    }
}
