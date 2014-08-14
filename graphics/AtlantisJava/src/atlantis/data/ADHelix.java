package atlantis.data;

import java.awt.geom.Point2D;
import java.util.Vector;
import java.awt.Rectangle;

import atlantis.canvas.AWindow;
import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.graphics.ACoord;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjection3D;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionXZ;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;

/**
 * Helix in a format suitable for drawing given as a circle with
 * centre point and radius etc.
 * 
 * phi is the angle seen from the primary vertex
 * a = alpha = the angle as seen for the center of the circle
 * 
 */
public class ADHelix
{
    private final static ALogger logger = ALogger.getLogger(ADHelix.class);

    // parameters used internally
    private static final APar parameterStore = APar.instance();
    private static final AParameter rToPar = parameterStore.get("RTr", "RadiusTr");
    private static final AParameter lToPar = parameterStore.get("RTr", "LineTr");
    private static final AParameter zToPar = parameterStore.get("RTr", "ZTr");
    private static final int callDepthCut = parameterStore.get("RTr", "CallDepth").getI();;
  
    /** The associated helix object. */
    public final AHelix helix;

    // description of helix
    private final double rC, xC, yC, ch, a0, x0, y0, z0, bz, cosA0, sinA0;
    // position along helix
    private final double aStart, aEnd;
    
    /** Attempt to encapsulate the "current position" variables. */
    private static class HelixPoint {
    	double x, y, z, rho, phi;
    }
    
    private double startPhi;    // the phi value of starting point of "v"
    // TODO: Would prefer this not to be part of the state of the helix object.
    private boolean startMark; // true when calculating start point, false otherwise

    ADHelix(AOldHelix h) {
        this(new AHelix(h));
    }
    
    ADHelix(AHelix h)
    {
    	helix = h;
        double[] perigee = h.getPar(); // d0, z0, phi0, cotanTheta, q/pT
      
        AParameter curvatureParameter = parameterStore.get("Event", "Curvature");
        if (curvatureParameter != null) {
            rC = curvatureParameter.getD() / Math.abs(perigee[4]);
        } else {
            rC = (100/0.6) / Math.abs(perigee[4]);
        }
        ch = AMath.getSign(perigee[4]);
        a0 = Math.toDegrees(perigee[2]) + ch * 90.;
        double a0Rad = Math.toRadians(a0);
        cosA0 = Math.cos(a0Rad);
        sinA0 = Math.sin(a0Rad);
        z0 = perigee[1];
        double d0 = perigee[0];
        double signD0 = ch;
        d0 *= signD0;
        x0 = d0 * cosA0;
        y0 = d0 * sinA0;
        double SD0 = 1.;
        double SRC = 1.;
        double r0 = SD0 * d0 - SRC * rC;
        xC = r0 * cosA0;
        yC = r0 * sinA0;
        bz = rC * Math.toRadians(perigee[3]);
        double rTo = rToPar.getD();
        //Check if track is limited by end vertex
        if ( helix.getRhoEndVertex() > 0. )
            rTo = Math.min(rTo,helix.getRhoEndVertex());
        double zTo = zToPar.getD();
        /*******************************************************************************/
        //Adrian Vatchinsky, Andy Haas (NYU)
        //Import the coordinates of the primary vertex in order to calculate new starting angle
        AEventManager eventManager = AEventManager.instance();
        AEvent currentEvent = eventManager.getCurrentEvent();
        if (helix.getRho(helix.getAlphaMin())!=0 || currentEvent==null) {  // FIXME: doubles compared for equality
        	aStart = helix.getAlphaMin();                     // old, DCA to 0,0
        } else {                                         // new, DCA to PV
            double[] primaryVertex = currentEvent.getPrimaryVertex();
            double xV = primaryVertex[0]; double yV = primaryVertex[1];
            aStart = helix.getAlphaMin() + ch*Math.toDegrees(Math.atan2(yC,xC)-Math.atan2(yC-yV,xC-xV));
        }
        /*******************************************************************************/        
        HelixPoint point = pointAt(aStart);
        // don't draw tracks starting outside the tracking region
        if(Math.abs(point.rho) > rTo || Math.abs(point.z) > zTo)
        {
            this.aEnd = aStart;
            return;
        }
        //Calculate max phi angle -> no track can have more than 180 deg
        double aEndMax = Math.min(aStart + 180., 180.);
        //Also the track should not stick out of the rTo zylinder
        double aEndR = intersectWithRadialCylinder(rTo, aStart, aEndMax);
        // nor should it go beyond the max. z-extension
        double aEndZ = intersectWithZPlanes(zTo, aStart, aEndMax);        // or beyond the end-vertex radius, if given
        double aEndTmp = Math.min(aEndMax, aEndR);
        if(aEndZ > aStart) aEndTmp = Math.min(aEndTmp, aEndZ);
        this.aEnd = Math.min(aEndTmp, aEndMax);
    }


    // calculate phi on track at intersection
    // Cylinder is infinite in Z direction
    public double intersectWithRadialCylinder(double rCyl, double aStart,
                                              double aEnd)
    {
        if(rC <= 0.) return aStart;
        double r0 = Math.sqrt(xC * xC + yC * yC);
        double bb = (rCyl * rCyl - rC * rC - r0 * r0) / (2. * rC * r0);
        double rhoStart = getRho(aStart);
        double rhoEnd = getRho(aEnd);
        if(rCyl >= rhoStart && rCyl >= rhoEnd) return aEnd;
        if(rCyl <= rhoStart && rCyl <= rhoEnd) return aStart;
        if(Math.abs(bb) > 1.)
        {
            // needs to be understood better (Hans)
            // event file: acr_muontrack_problem_event.xml
            logger.warn("Bug in ADHelix " + rCyl + " " + rhoStart + " " + rhoEnd);
            return aEnd;
        }
        double cc = Math.toDegrees(Math.acos(bb));
        if(Math.abs(cc) > 360)
            throw new Error("error in cc in dhelix");
        double gA = Math.toDegrees(Math.atan2(yC, xC));
        double a1 = ch * (a0 - gA + cc);
        double a2 = ch * (a0 - gA - cc);
        if(a1 < 0.) a1 += 360.;
        if(a2 < 0.) a2 += 360.;
        if(a1 >= 360.) a1 -= 360.;
        if(a2 >= 360.) a2 -= 360.;
        return Math.min(a1, a2);
    }


    // calculate phi on track at intersection
    public double intersectWithZPlanes(double zCyl, double aStart, double aEnd)
    {
        double zMin = 20.;
        double aZ = aEnd;
        if(zCyl >= zMin && bz != 0.)
        {
            double aZPlus = (zCyl - z0) / bz;
            double aZMinus = ( -zCyl - z0) / bz;
            if(aZPlus < aZ && aZPlus > aStart) aZ = aZPlus;
            if(aZMinus < aZ && aZMinus > aStart) aZ = aZMinus;
        }
        return aZ;
    }


    // calculate phi on track at intersection
    // cylinder finite in Z
    public double intersectWithCylinder(boolean useR, double rCyl, boolean useZ,
                                        double zCyl)
    {
        double aStart = 0.;
        double aEndMax = 180.;
        double zMin = 20.;
        double aEndR = 0.;
        if(useR)
            aEndR = intersectWithRadialCylinder(rCyl, aStart, aEndMax);
        double aEndZ = 0.;
        if(useZ && zCyl >= zMin && bz != 0.)
        {
            aEndZ = intersectWithZPlanes(zCyl, aStart, aEndMax);
            return Math.min(aEndR, aEndZ);
        }
        else
            return aEndR;
    }


    // a... angle in deg starting with 0 meaning point of closest approach
    private HelixPoint pointAt(double a)
    {
    	HelixPoint point = new HelixPoint();
    	double aRad = Math.toRadians(a0 - ch * a);
    	point.x = x0 + rC * (Math.cos(aRad) - cosA0);
    	point.y = y0 + rC * (Math.sin(aRad) - sinA0);
    	point.z = z0 + bz * a;
    	double[] primaryVtx = AParameterUtilities.getPrimaryVertex();
    	double dx = point.x - primaryVtx[0];
    	double dy = point.y - primaryVtx[1];
    	point.rho = Math.sqrt(dx * dx + dy * dy);
    	double phi0 = Math.toDegrees(Math.atan2(dy, dx));
        double phiMid2 = parameterStore.get("RZ", "Phi").getD();
    	point.phi = AMath.nearestPhiDegrees(phi0,phiMid2);
    	return point;
    }


    // s1 and s2 are the extreme values of phi
    // used to avoid tracks being drawn with phi discontinuity
    // in phi projections
    // = FiMid in atlantis fortran ask Hans Drevermann
    public double setPhiStart(double s1, double s2)
    {
        double phi1 = getPhi(s1);
        double phi2 = getPhi(s2);
        double phiM = getPhi((s1 + s2) / 2);
        // doesn't work for all tracks but for most
        if(Math.abs(phi1 - phi2) > 180. || Math.abs(phi1 - phiM) > 180. ||
           Math.abs(phi2 - phiM) > 180.)
        {
            if(phi1 - phiM > 180.) phi1 -= 360.;
            if(phi2 - phiM > 180.) phi2 -= 360.;
            if(phi1 - phiM < -180.) phi1 += 360.;
            if(phi2 - phiM < -180.) phi2 += 360.;

            // must be something wrong here, phiM is the same using the
            // following code, then why use if ... else ...
            if(phi1 < phiM && phiM < phi2)
                phiM = (phi1 + phi2) / 2.;
            else if(phi1 > phiM && phiM > phi2)
                phiM = (phi1 + phi2) / 2.;
            else
            {
                phiM = (phi1 + phi2) / 2.;
            }
        }
        if(phiM > 360.)
            phiM -= 360.;
        if(phiM < 0.)
            phiM += 360.;
        return phiM;
    }


    public double getAStart()
    {
        return aStart;
    }


    public double getAEnd()
    {
        return aEnd;
    }


    public ACoord getYXPoint(double a)
    {
    	HelixPoint point = pointAt(a);
        return new ACoord(point.x, point.y, -1);
    }


    public ACoord getRZPoint(double a)
    {
    	HelixPoint point = pointAt(a);
        double phiMid2 = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
        double phiDiff = Math.abs(Math.toRadians(point.phi) - phiMid2);
        if(phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
            return new ACoord(point.z, point.rho, -1);
        else
            return new ACoord(point.z, -point.rho, -1);
    }


    public ACoord get3DPoint(double a)
    {
    	HelixPoint point = pointAt(a);
        double[] point3D = {point.x,point.y,point.z};
        point3D = AProjection3D.getRotated(point3D);
        return new ACoord(point3D[0], point3D[1], -1);
    }


    public double[] get3DPointAsArray(double a)
    {
    	HelixPoint point = pointAt(a);
        double[] point3D = new double[] {point.x, point.y, point.z };
        point3D = AProjection3D.getRotated(point3D);
        return point3D;
    }


    public ACoord getXZPoint(double a)
    {
    	HelixPoint point = pointAt(a);
        double phiC = AProjectionXZ.getPhi();
        return new ACoord(point.z, point.rho * Math.cos(Math.toRadians(point.phi - phiC)), -1);
    }


    public ACoord getYZPoint(double a)
    {
    	HelixPoint point = pointAt(a);
        double phiC = AProjectionXZ.getPhi();
        return new ACoord(point.z, point.rho * Math.sin(Math.toRadians(point.phi - phiC)), -1);
    }


    public ACoord getFZPoint(double a)
    {
    	HelixPoint point = pointAt(a);
        double temp = Math.toDegrees(AParameterUtilities.getPhiStereo(point.rho, Math.toRadians(point.phi), point.z));
        return new ACoord(point.z, temp, -1);
    }


    public ACoord getFRPoint(double a)
    {
    	HelixPoint point = pointAt(a);
        double temp = Math.toDegrees(AParameterUtilities.getPhiStereo(point.rho, Math.toRadians(point.phi), point.z));
        return new ACoord(point.rho, temp, -1);
    }


    public ACoord getVPPoint(double a, int sign)
    {
    	HelixPoint point = pointAt(a);
        if(startMark)   // start point
        {
            startPhi = point.phi; // phi is between 0 and 360
        }
        else
        {
        	point.phi = AMath.nearestPhiDegrees(point.phi, startPhi);
        }
        return new ACoord(AParameterUtilities.eta(point.z, point.rho) +
                          sign * AProjectionVP.getDeltaEta(point.rho, point.z), point.phi, -1);
    }


    public ACoord getLEGOPoint(double a)
    {
        //added by Mark Stockton
    	HelixPoint point = pointAt(a);
        double temp = Math.toDegrees(AParameterUtilities.getPhiStereo(point.rho, Math.toRadians(point.phi), point.z));
        return new ACoord(AParameterUtilities.eta(point.z, point.rho), temp, -1);
    }


    public double getPhi(double a)
    {
    	HelixPoint point = pointAt(a);
        return point.phi;
    }


    public double getEta(double a)
    {
    	HelixPoint point = pointAt(a);
        return AParameterUtilities.eta(point.z, point.rho);
    }


    private double getRho(double a)
    {
    	HelixPoint point = pointAt(a);
        return point.rho;
    }


    /** Draw helix in display space in an iterative manner. */
    public ACoord drawHelix(AWindow window, AProjection2D projection, double s1, double s2)
    {
        Vector<Point2D.Double> pointsOnHelix = new Vector<Point2D.Double>(100);
        double dPrevious = 999.;
        int callDepth = 0;
        // get the point at the beginning and end of the helix, defined by the phi-values (s1,s2)
        startMark = true;
        ACoord d1 = window.calculateDisplay(projection.getUserPoint(this, s1));
        double h1 = d1.hv[0][0][0];
        double v1 = d1.hv[1][0][0];
        startMark = false;
        ACoord d2 = window.calculateDisplay(projection.getUserPoint(this, s2));
        double h2 = d2.hv[0][0][0];
        double v2 = d2.hv[1][0][0];
        pointsOnHelix.add(new Point2D.Double(h1, v1));
        // We are simply adding points here, in the end the helix is drawn as a polyline
        drawHelix(pointsOnHelix, window, projection, s1, h1, v1, s2, h2, v2,
                  dPrevious, callDepth);
        int numPoints;
        if(projection instanceof AProjectionVP)
        {
            numPoints = pointsOnHelix.size();
        }
        else
        {
            numPoints = pointsOnHelix.size() + 1; // add 1 to add a point for the line
        }
        double[][][] hv = new double[2][1][numPoints];
        int[] index = new int[1];
        Point2D.Double p;
        if(projection instanceof AProjectionVP)
        {
            for(int k = 0; k < numPoints; ++k)
            {
                p = (Point2D.Double) pointsOnHelix.elementAt(k);
                hv[0][0][k] = p.getX();
                hv[1][0][k] = p.getY();
            }
        }
        else
        {
            for(int k = 0; k < numPoints - 1; ++k)
            {
                p = (Point2D.Double) pointsOnHelix.elementAt(k);
                hv[0][0][k] = p.getX();
                hv[1][0][k] = p.getY();
            } // first attempt for line...
            if(numPoints > 2)
            {
                double dx = hv[0][0][numPoints - 2] - hv[0][0][numPoints - 3];
                double dy = hv[1][0][numPoints - 2] - hv[1][0][numPoints - 3];
                double x0 = hv[0][0][numPoints - 2] - hv[0][0][0];
                double y0 = hv[1][0][numPoints - 2] - hv[1][0][0];

                // InDet -> Track -> Rho Track linearly is 0 by default
                // and the value is meant beyond Rho Track as Helix parameter,
                // hence rToPar + lToPar is used instead of former lToPar only
                double dtopar = ( (rToPar.getD() + lToPar.getD()) / rToPar.getD()) *
                                Math.sqrt(x0 * x0 + y0 * y0);
                double cophi = dx / Math.sqrt(dx * dx + dy * dy);
                double siphi = dy / Math.sqrt(dx * dx + dy * dy);
                double addl = Math.sqrt((y0 * siphi + x0 * cophi) *
                                        (y0 * siphi + x0 * cophi) -
                                        x0 * x0 - y0 * y0 + dtopar * dtopar);
                double addlen = Math.min(Math.abs( -(y0 * siphi + x0 * cophi) +
                                                  addl),
                                         Math.abs( -(y0 * siphi + x0 * cophi) -
                                                  addl));
                hv[0][0][numPoints - 1] = x0 + addlen * cophi + hv[0][0][0];
                hv[1][0][numPoints - 1] = y0 + addlen * siphi + hv[1][0][0];
            }
        }
        return new ACoord(hv, index);
    }


    /** Called recursively. */
    private void drawHelix(Vector<Point2D.Double> pointsOnHelix, AWindow window, AProjection2D projection,
                           double s1, double h1, double v1, double s2, double h2, double v2,
                           double dPrevious, int callDepth)
    {
        double D_MAX = 1.;
        double h21 = h2 - h1;
        double v21 = v2 - v1;
        // nothing left to draw
        if(h21 == 0. && v21 == 0.)
            return;
        // calculate midpoint
        double sM = 0.5 * (s1 + s2);
        ACoord dispM = window.calculateDisplay(projection.getUserPoint(this, sM));
        double hM = dispM.hv[0][0][0];
        double vM = dispM.hv[1][0][0];

        // For speed reasons we make sure we aren't doing a lot of subdivison
        // on helix segments outside the view area
        boolean isinview = true;

        if (callDepth > 2) {
            // Check if bounding box of segment is on the screen anywhere
            // Don't try this until we've divided a few times just in case we
            // have a really curly helix to deal with
            Rectangle wdim = window.getCurrDisp();

            int left = (int)h1;
            int top = (int)v1;
            int width = (int)(h2 - h1);
            int height = (int)(v2 - v1);

            // Make sure the box isn't upside down or zero in size as this
            // seemed to be causing a few problems
            if (width < 0) { left += width; width = -width; }
            if (height < 0) { top += height; height = -height; }
            if (width == 0) { width = 1; }
            if (height == 0) { height = 1; }

            Rectangle bound = new Rectangle(left, top, width, height);

            if (!wdim.intersects(bound)) {
                // Prevent any further sub-division
                isinview = false;
            }
        }

        // calculate distance of midpoint from straight line approximation
        double hM1 = hM - h1;
        double vM1 = vM - v1;
        double temp = (v21 * hM1 - h21 * vM1);
        double dM = temp * temp / (v21 * v21 + h21 * h21);
        // this iteration and previous one within tolerance
        if(dPrevious < D_MAX && dM < D_MAX)
        {
            pointsOnHelix.add(new Point2D.Double(h2, v2));
        }
        else
        {
            if(callDepth < callDepthCut && isinview)
            {
                // split track in two and iterate
                drawHelix(pointsOnHelix, window, projection, s1, h1, v1, sM, hM,
                          vM, dM, callDepth + 1);
                drawHelix(pointsOnHelix, window, projection, sM, hM, vM, s2, h2,
                          v2, dM, callDepth + 1);
            }
            else
            {
                pointsOnHelix.add(new Point2D.Double(hM, vM));
                pointsOnHelix.add(new Point2D.Double(h2, v2));
            }
        }
    }

	public double getxC() {
		return xC;
	}

	public double getyC() {
		return yC;
	}
}
