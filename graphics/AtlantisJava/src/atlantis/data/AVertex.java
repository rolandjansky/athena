package atlantis.data;

import Jama.Matrix;

import java.util.HashMap;
import java.util.Map;
import atlantis.parameters.APar;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjectionXZ;
import atlantis.projection.AProjectionYX;
import atlantis.projection.AProjectionYZ;
import atlantis.utils.AMath;
import atlantis.utils.AAtlantisException;

/**
 *
 *    Reconstructed vertex instance. Either calculated in
 *    Atlantis (atlantis.utils.AVertexFit class) or read in from the event data as
 *    RVx datatype. RVx class keeps track of all the reconstructed vertices
 *    and contains instances of this class Avertex.
 *    (2005-08-08 refactoring and clean-ups)
 *    (2006-01-10 covMatrix, errMatrix variables were messed up (covMatrix
 *    numbers were huge - was actually the inverse - the weight matrix. AVertexFit
 *    code didn't give any ideas, very unclear code  - covFittedMatrix there
 *    was used to initialize errMatrix here ...))
 *
 *
 *    variables names renaming:
 *    covMatrix -> weightMatrix
 *    errMatrix -> covErrorMatrix
 *
 * @author Gary Taylor
 */
public class AVertex
{
    // size of the arrays with coordinates of the RVx ellipse and matrices
    private static final int SIZE = 3;

    // basic RVx data
    private double[] position = null; // x, y, z centre of the ellipse
    private double chi2 = 1.e+10;
    private Matrix covErrMatrix = null; // covariance error matrix
    private Matrix weightMatrix = null; // inverse matrix of covErrMatrix
    private double phi; // phi of the ellipse
    private double rho; // rho of the ellipse
    
    private static APar parameterStore = APar.instance();

    public AVertex(double[] p)
    {
        position = new double[SIZE];

        if (p.length != SIZE)
        {
            throw new IllegalArgumentException("p.length=" + p.length);
        }
        for (int i = 0; i < SIZE; ++i)
        {
            position[i] = p[i];
        }
    } // AVertex() ----------------------------------------------------------

    /**
     * @param p double[] - x, y, z positions of the centre of the ellipse
     * @param c double[][] - input for covErrMatrix
     * @param chiSquared double
     */
     public AVertex(double[] p, double[][] c, double chiSquared)
     {      
        testInputArrays(p, c);

        position = new double[SIZE];
        double[][] covErr = new double[SIZE][SIZE];

        for (int i = 0; i < SIZE; ++i)
        {
            position[i] = p[i];
        }
        phi = Math.atan2(p[1], p[0]);
        if (phi < 0.)
            phi += AMath.TWO_PI;
        rho = Math.sqrt(p[0] * p[0] + p[1] * p[1]);
        for (int i = 0; i < SIZE; ++i)
        {
            for (int j = 0; j < SIZE; ++j)
            {
                covErr[i][j] = c[i][j];
            }
        }

        covErrMatrix = new Matrix(covErr);
        weightMatrix = covErrMatrix.inverse();
        this.chi2 = chiSquared;
    } // AVertex() ----------------------------------------------------------

    /**
     *
     * @param chiSquared double
     * @param p double[] - x, y, z positions of the centre of the ellipse
     * @param c double[][] - input for covErrMatrix
     */
    public AVertex(double chiSquared, double[] p, double[][] c) throws AAtlantisException
    {
        testInputArrays(p, c);

        chi2 = chiSquared;

        position = new double[SIZE];

        for (int i = 0; i < SIZE; ++i)
        {
            position[i] = p[i];
        }

        phi = Math.atan2(p[1], p[0]);
        if (phi < 0.)
            phi += AMath.TWO_PI;
        rho = Math.sqrt(p[0] * p[0] + p[1] * p[1]);

        double[][] covErr = new double[SIZE][SIZE];
        for (int x = 0; x < SIZE; x++)
        {
            covErr[x] = (double[]) c[x].clone();
        }

        try
        {
            covErrMatrix = new Matrix(covErr);
            weightMatrix = covErrMatrix.inverse();
        }
        catch (Exception ex)
        {
            String msg = "Error when creating covariance and/or weight\n" + "matrix in AVertex class.\nReason: " + ex.getMessage();
            throw new AAtlantisException(msg);
        }
    } // AVertex() ----------------------------------------------------------

    private void testInputArrays(double[] p, double[][] c) throws IllegalArgumentException
    {
        if (p.length != SIZE)
        {
            throw new IllegalArgumentException("p.length = " + p.length);
        }

        if (c.length != SIZE)
        {
            throw new IllegalArgumentException("c.length = " + c.length);
        }

        for (int i = 0; i < SIZE; ++i)
        {
            if (c[i].length != SIZE)
            {
                throw new IllegalArgumentException("c[" + i + "].length = " + c[i].length);
            }
        }
    } // testInputArrays() --------------------------------------------------

    public double[] getPosition()
    {
        return (double[]) position.clone();
    } // getPosition() ------------------------------------------------------

    public Matrix getCovErrMatrix()
    {
        return covErrMatrix;
    } // getCovErrMatrix() --------------------------------------------------

    public double getRho()
    {
        return rho;
    }

    public double getPhi()
    {
        return phi;
    }

    public String toString()
    {
        StringBuffer msg = new StringBuffer(" x = ");
        msg.append(String.format("%.5f",position[0]));
        msg.append(" ");
        msg.append(AMath.PLUSMINUS);
        msg.append(" ");
        msg.append(String.format("%.3f",Math.sqrt(covErrMatrix.get(0, 0))));
        msg.append(" cm\n y = ");
        msg.append(String.format("%.5f",position[1]));
        msg.append(" ");
        msg.append(AMath.PLUSMINUS);
        msg.append(" ");
        msg.append(String.format("%.3f",Math.sqrt(covErrMatrix.get(1, 1))));
        msg.append(" cm\n z = ");
        msg.append(String.format("%.5f",position[2]));
        msg.append(" ");
        msg.append(AMath.PLUSMINUS);
        msg.append(" ");
        msg.append(String.format("%.3f",Math.sqrt(covErrMatrix.get(2, 2))));
        msg.append(" cm\n ");
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.4f",AParameterUtilities.eta(position[2], rho)));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.1f",Math.toDegrees(phi)));
        msg.append(AMath.DEGREES);
        msg.append("\n chi2 = ");
        msg.append(String.format("%.1f",chi2));

        return msg.toString();
    } // toString() ---------------------------------------------------------

    // Ask Alan Litke to explain this calculation
    private double[][] getEllipse(Matrix rM, int[] axisMapping)
    {
        Matrix parPrime = rM.times(new Matrix(position, position.length));

        double a1 = weightMatrix.get(axisMapping[0], axisMapping[0]);
        double a2 = weightMatrix.get(axisMapping[1], axisMapping[1]);
        double a3 = weightMatrix.get(axisMapping[2], axisMapping[2]);
        double a4 = 2. * weightMatrix.get(axisMapping[0], axisMapping[1]);
        double a5 = 2. * weightMatrix.get(axisMapping[0], axisMapping[2]);
        double a6 = 2. * weightMatrix.get(axisMapping[1], axisMapping[2]);
        double b1 = a1 - a5 * a5 / (4. * a3);
        double b2 = a2 - a6 * a6 / (4. * a3);
        double b3 = a4 - 2 * a5 * a6 / (4. * a3);
        double[][] hv = new double[2][360];
        double sigma = parameterStore.get("RVx", "NumSigma").getD();
        for (int i = 0; i < 360; ++i)
        {
            double thetat = Math.toRadians(i);
            double tt = Math.tan(thetat);
            double x = Math.sqrt(sigma * sigma / (b1 + b2 * tt * tt + b3 * tt));
            if (i > 90 && i <= 270)
                x *= -1.;
            hv[0][i] = x + parPrime.get(axisMapping[0], 0);
            hv[1][i] = x * tt + parPrime.get(axisMapping[1], 0);
        }
        return hv;
    } // getEllipse() -------------------------------------------------------

    public double[][] getYXEllipse()
    {
        Matrix rM = new Matrix(AProjectionYX.getRotationMatrix());
        int[] axisMapping = AProjectionYX.getAxisMapping();

        return getEllipse(rM, axisMapping);
    } // getYXEllipse() -----------------------------------------------------

    public double[][] getYZEllipse()
    {
        Matrix rM = new Matrix(AProjectionYZ.getRotationMatrix());
        int[] axisMapping = AProjectionYZ.getAxisMapping();

        return getEllipse(rM, axisMapping);
    } // getYZEllipse() -----------------------------------------------------

    public double[][] getXZEllipse()
    {
        Matrix rM = new Matrix(AProjectionXZ.getRotationMatrix());
        int[] axisMapping = AProjectionXZ.getAxisMapping();

        return getEllipse(rM, axisMapping);
    } // getXZEllipse() -----------------------------------------------------

    public double[][] getRZEllipse()
    {
        double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
        double phiDiff = Math.abs(phi-phiMid);
        int rSign; if (phiDiff > Math.PI/2. && phiDiff <= 3*Math.PI/2.) rSign = -1; else rSign = 1;
        
        double[][] hv = new double[2][360];
        double sigma = parameterStore.get("RVx", "NumSigma").getD();
        for (int i = 0; i < 360; ++i)
        {
            double thetat = Math.toRadians(i);
            double ey = Math.sqrt(Math.sqrt(covErrMatrix.get(0,0)*covErrMatrix.get(0,0)+covErrMatrix.get(1,1)*covErrMatrix.get(1,1)));
            double ex = Math.sqrt(covErrMatrix.get(2,2));
            hv[1][i] = rSign * Math.sqrt(position[0]*position[0]+position[1]*position[1]) + sigma*ey*Math.sin(thetat);
            hv[0][i] = position[2] + sigma*ex*Math.cos(thetat);
            //System.out.println(i+" "+thetat+" "+hv[0][i]+" "+hv[1][i]);
        }
        return hv;
    } // getXZEllipse() -----------------------------------------------------
    
    // Vertex type - enum same as in Athena
   
    enum Type {
    	DUMMY (0, "Dummy. Track particle was not used in vertex fit."),
    	PRIMARY (1, "Primary"),
    	SECONDARY (2, "Secondary"),
    	PILEUP (3, "Pile-up"),
    	CONVERSION (4, "Conversion"),
    	V0 (5, "V0 decay"),
    	KINK (6, "Kink"),
    	TEMPV0LAMBDA (7, "Temporary Addition for V0 Lambda"),
    	TEMPV0LAMBDABAR (8, "Temporary Addition for V0 LambdaBar"),
    	TEMPKSHORT (9, "Temporary Addition for KShort"),
    	DEFAULT (99, "Not specified");

    	public final int id;
    	public final String description;
    	
    	private static Map<Integer,Type> typeMap = new HashMap<Integer,Type>();
    	static {
    		for (Type type : Type.values()) {
    			typeMap.put(type.id, type);
    		}
    	}
    
    	/**
    	 * Get vertex type corresponding to given integer type identifier.
    	 * @param id type identifier
    	 * @return vertex type
    	 */
    	public static Type typeFromId(int id) {
    		return typeMap.get(id);
    	}
    	
    	private Type (int id, String description) {
    		this.id = id;
    		this.description = description;
    	}
    }
    
}   
   