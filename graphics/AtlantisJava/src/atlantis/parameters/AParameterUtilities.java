/**
 * 
 */
package atlantis.parameters;

import atlantis.utils.AMath;

/**
 * Mathematical utility methods that act on Parameters.
 * Moved from atlantis.utils.AMath to reduce dependencies of utils on other packages.
 *
 * @author waugh
 *
 */
public class AParameterUtilities {

	private final static APar parameterStore = APar.instance();
	private final static AParameter xVertexPar = parameterStore.get("Event", "XVtx");
	private final static AParameter yVertexPar = parameterStore.get("Event", "YVtx");
	private final static AParameter zVertexPar = parameterStore.get("Event", "ZVtx");

	/**
	 * Get coordinates of event vertex.
	 * 
	 * @return (x,y,z) of vertex
	 */
    public static double[] getPrimaryVertex()
    {
        double[] primaryVertex = new double[3];
        primaryVertex[0] = parameterStore.get("Event", "XVtx").getD();
        primaryVertex[1] = parameterStore.get("Event", "YVtx").getD();
        primaryVertex[2] = parameterStore.get("Event", "ZVtx").getD();
        return primaryVertex;
    }

	/**
	 * Returns phi with respect to the primary vertex.
	 * @param x double x coordinate
	 * @param y double y coordinate
	 * @return double phi
	 */
	public static double phi(double x, double y) {
	    double phi = Math.atan2(y-yVertexPar.getD(), x-xVertexPar.getD());
	    if (phi < 0) phi += 2.*Math.PI;
	    return Math.toDegrees(phi);
	}

	/**
	 * Returns eta with respect to the primary vertex.
	 * @param z double z coordinate
	 * @param rho double rho coordinate
	 * @return double eta
	 */
	public static double eta(double z, double rho)
	{
	    return AMath.etaAbs(z - zVertexPar.getD(), rho);
	}

	public static double getPhiStereo(double rho, double phi, double z)
	{
	    AParameter stereoAnglePar = parameterStore.get("SiCluster", "Stereo");
	    if(!stereoAnglePar.getStatus())
	    {
	        return phi;
	    }
	    double stereoAngle = stereoAnglePar.getI() * 0.04;
	    return phi + stereoAngle * (z-parameterStore.get("Event", "ZVtx").getD()) / rho;
	}

	/**
	 * Get the sign of the rho coordinate based on the x and y coordinates and
	 * the phi value set by the user.
	 * @param x double x coordinate
	 * @param y double y coordinate
	 * @return int sign of rho (-1 or +1)
	 */
	public static int getRhoSign(double x, double y) {
	    return getRhoSign(Math.atan2(y, x));
	}

	/**
	 * Get the sign of the rho coordinate based on the phi coordinate and
	 * the phi value set by the user.
	 * @param phi double phi coordinate
	 * @return int sign of rho (-1 or +1)
	 */
	private static int getRhoSign(double phi) {
	    int sign;
	    if (phi < 0.) phi += 2.*Math.PI;
	
	    double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
	    double phiDiff = Math.abs(phi-phiMid);
	
	    if(phiDiff <= Math.PI/2.|| phiDiff > 3*Math.PI/2.) {
	        sign = 1;
	    } else {
	        sign = -1;
	    }
	    return sign;
	}

}
