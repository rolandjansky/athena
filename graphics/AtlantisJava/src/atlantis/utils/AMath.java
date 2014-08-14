package atlantis.utils;

import java.awt.geom.Point2D;


/**
 * Contains generally useful constants and mathematical functions
 * and functions for formatting
 */
public class AMath
{
    final public static double TWO_PI = 2. * Math.PI;
    final public static double PI_BY_8 = Math.PI / 8.;

    // Constant giving the unicode value of a small Greek eta.
    final static public String ETA = "\u03B7";
    // Constant giving the unicode value of a small Greek lambda.
    final static public String LAMBDA = "\u03BB";
    // Unicode capital Lambda
    final static public String LAMBDACAP = "\u039B";
    // Unicode lowercase gamma
    final static public String GAMMA = "\u03B3";
    // Constant giving the unicode value of a small Greek rho.
    final static public String RHO = "\u03C1";
    // Constant giving the unicode value of a small Greek phi.
    // final static public String PHI = "\u03C6"; // small
    final static public String PHI = "\u03A6"; // capital
    // Constant giving the unicode value of degrees symbol.
    final static public String DEGREES = "\u00B0";
    // Constant giving the unicode value of a capital Greek delta.
    final static public String DELTA = "\u0394";
    // final static public String PRIME = "\u00b4";
    final static public String PRIME = "'";
    final static public String MICRO = "\u00b5";
    final static public String TIMES = "\u00d7";
    final static public String ASYMP = "\u2248";
    final static public String PROP = "\u221D";
    final static public String RADIC = "\u221A";

    final static public String PLUSMINUS = "\u00b1";
    final static public String NOTEQUAL = "\u2260";
    final static public String LESSEQUAL = "\u2264";
    final static public String GREATEREQUAL = "\u2265";
    final static public String PION = "\u03C0";
    final static public String SIGMA = "\u03A3";
    final static public String TAU = "\u03C4";
    final static public String XI = "\u039E";
    final static public String OMEGA = "\u03A9";
    final static public String MU = "\u03BC";
    
    final static public String RARROW = "\u279D"; // right arrow
    final static public String DOT = "\u00b7"; // middle dot


    /**
     * Use the unicode overline diacritical mark to place an overline over the preceeding character
     * 
     */
    final static public String OVERLINE = "\u0305";
    // superscript plus and minus
    final static public String SUPPLUS = "\u207A";
    final static public String SUPMINUS = "\u207B";
    

    /**
     * Returns eta with respect to z = 0.
     * @param z double z coordinate
     * @param rho double eta coordinate
     * @return double eta
     */
    public static double etaAbs(double z, double rho) {
        double zrho = z / rho;
        return Math.log(zrho + Math.sqrt(zrho * zrho + 1.));
    }

    public static double tanLambda(double eta)
    {
        double e = Math.exp(eta);
        return 0.5 * (e-1./e);
    }

    public static double lambda(double eta)
    {
        double e = Math.exp(eta);
        return Math.atan(0.5 * (e-1./e));
    }

    public static double getPFromPttL(double pT, double tL)
    {
        return Math.abs(pT / Math.cos(Math.atan(tL)));
    }

    public static double getPtFromPtL(double p, double tL)
    {
        return Math.abs(p * Math.cos(Math.atan(tL)));
    }

    /**
     * Force phi into range [0,360) degrees.
     * @param phi angle in degrees
     * @return equivalent angle in range [0,360) degrees
     */
    public static double nearestPhiDegrees(double phi)
    {
        while(phi >= 360.)
        {
            phi -= 360;
        }
        while(phi < 0.)
        {
            phi += 360;
        }
        return phi;
    }

    /**
     * Force phi into range [0,2 pi) radians.
     * @param phi angle in radians
     * @return equivalent angle in range [0,2 pi) radians
     */
    public static double nearestPhiRadians(double phi)
    {
        while(phi >= TWO_PI)
        {
            phi -= TWO_PI;
        }
        while(phi<0.)
        {
            phi += TWO_PI;
        }
        return phi;
    }

    /**
     * Force phi into range [phiMid-180,phiMid+360) degrees.
     * @param phi angle in degrees
     * @param phiMid mid-point of required phi range
     * @return equivalent angle in range [phiMid-180,phiMid+360) degrees
     */
    public static double nearestPhiDegrees(double phi, double phiMid)
    {
        while(phi-phiMid >= 180.)
        {
            phi -= 360;
        }
        while(phi-phiMid < -180.)
        {
            phi += 360;
        }
        return phi;
    }

    /**
     * Force phi into range [phiMid-pi,phiMid+pi) radians.
     * @param phi angle in degrees
     * @param phiMid phiMid mid-point of required phi range
     * @return equivalent angle in range [phiMid-pi,phiMid+pi) radians
     */
    public static double nearestPhiRadians(double phi, double phiMid)
    {
        while(phi - phiMid >= Math.PI)
        {
            phi -= TWO_PI;
        }
        while(phi - phiMid < -Math.PI)
        {
            phi += TWO_PI;
        }
        return phi;
    }

    public static double[] splitLineIntoPieces(double[] input, int numPieces)
    {
        if(input.length != 2)
        {
            return input;
        }
        double[] output = new double[numPieces + 1];
        for(int p = 0; p < numPieces + 1; p++)
        {
            output[p] = input[0] +
            (input[1] - input[0]) * (double) p / (double) numPieces;
        }
        return output;
    }

/**
 * "Stretches" the values in the input array over a longer array.
 * A number numPieces[i] of equally spaced values between input[i] and input[i+1]
 * are put into the output array. So the numPieces array must have the same length
 * as the input array, and the length of the output array is the sum of the
 * elements of numPieces. For example:
 *  input = {0.0, 1.0, 5.0}   numPieces = {2, 4, 1}
 *    output = {0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0}
 * The output array must already exist and have the correct number of entries,
 * otherwise this method will do nothin.
 * Only used by ATGCData.getYXUser().
 * @param input array to stretch
 * @param output stretched array
 * @param numPieces number of output elements corresponding to each input element
 */
    public static void splitArrayIntoPieces(double[] input, double[] output, int[] numPieces)
    {
    	// Previously had "output = (double[]) input.clone();" but this is not
    	// passed back to the calling code so no point.
        if(input.length != numPieces.length) return;
        
        int totalNumPieces = 0;
        for (int num : numPieces) totalNumPieces += num;
        if(totalNumPieces != output.length) return;

        int index = 0;
        for(int i = 0; i < input.length; i++) {
            int nextI = (i + 1) % input.length; // "loop back" to start of input
            double start = input[i];
            double end = input[nextI];
            for(int p = 0; p < numPieces[i]; p++)
            {
                output[index++] = start +
                (end - start) * (double) p / (double) numPieces[i];
            }
        }
    }

    /**
     * Calculate angle between two 3-vectors represented by Point2D.Double
     * objects.
     * @param p1
     * @param p2
     * @return angle [radians]
     */
    public static double getAngle(Point2D.Double p1, Point2D.Double p2)
    {
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        return Math.atan2(dy, dx);
    }

    public static Point2D.Double intersectionPoint(
            Point2D.Double p1, Point2D.Double p2, Point2D.Double p3, Point2D.Double p4)
    {

        double u = ((p4.x-p3.x)*(p1.y-p3.y)-(p4.y-p3.y)*(p1.x-p3.x)) /
        ((p4.y-p3.y)*(p2.x-p1.x)-(p4.x-p3.x)*(p2.y-p1.y));
        return new Point2D.Double(p1.x+u*(p2.x-p1.x), p1.y+u*(p2.y-p1.y));
    }

    public static Point2D.Double intersectionPoint(
            Point2D.Double p1, AVector v1, Point2D.Double p3, AVector v3)
    {
        Point2D.Double p2 = new Point2D.Double(p1.x+v1.dx, p1.y+v1.dy);
        Point2D.Double p4 = new Point2D.Double(p3.x+v3.dx, p3.y+v3.dy);

        return intersectionPoint(p1, p2, p3, p4);
    }


    public static double[] xBox(double x,double dx)
    {
        return new double[] {x-dx,x-dx,x+dx,x+dx};
    }

    public static double[] yBox(double y,double dy)
    {
        return new double[] {y-dy,y+dy,y+dy,y-dy};
    }

    /**
     * Returns the sign of a double value.
     * Unlike Math.signum() it never returns zero.
	 */
    public static double getSign(double value) {
        return (value > 0) ? 1.0 : -1.0;
    }

    /**
     * Returns the sign of a int value.
     * Unlike Math.signum() it never returns zero.
     * 
     */
    public static int getSign(int value) {
        return (value > 0) ? 1 : -1;
    }

    public static float maxAbsInArray(float[] floatArray)
    {
        float maxAbs = Math.abs(floatArray[0]);
        for(int i=1; i<floatArray.length; ++i)
        {
            maxAbs = Math.max(Math.abs(floatArray[i]), maxAbs);
        }
        return maxAbs;
    }

    public static float minAbsInArray(float[] floatArray)
    {
        float minAbs = Math.abs(floatArray[0]);
        for(int i=1; i<floatArray.length; ++i)
        {
            minAbs = Math.min(Math.abs(floatArray[i]), minAbs);
        }
        return minAbs;
    }
    
    public static float minNonZeroAbsInArray(float[] floatArray)
    {
        float minAbs = Math.abs(floatArray[0]);
        for(int i=1; i<floatArray.length; ++i)
        {
            if(minAbs == 0.0f && floatArray[i] != 0.0f)
                minAbs = floatArray[i];
            else
                minAbs = Math.min(Math.abs(floatArray[i]), minAbs);
        }
        return minAbs;
    }
    
    public static float deltaR(float phi1, float eta1, float phi2, float eta2){
       
        float deta=Math.abs(eta1-eta2);
        float dphi=Math.abs(phi1-phi2);
        if(dphi>Math.PI)
            dphi=(float)(Math.PI*2.0f)-dphi;
        float dr=(float)Math.sqrt(deta*deta+dphi*dphi);
        return dr;
    }

} // class AMath -------------------------------------------------------------
