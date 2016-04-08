package atlantis.data;

import atlantis.event.*;
import atlantis.canvas.ACanvas;
import atlantis.utils.AMath;
import atlantis.utils.AHashMap;
import atlantis.utils.AAtlantisException;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AParameterUtilities;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.projection.AProjectionXZ;
import atlantis.graphics.AGraphics;
import atlantis.canvas.AWindow;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjection3D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.projection.AProjectionPhi;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionYX;
import atlantis.projection.AProjectionYZ;
import atlantis.projection.AProjectionsManager;
import atlantis.utils.A4Vector;
import atlantis.utils.ALogger;

/**
 *
 *    base class for ASTrData - Simulated Track
 *                   AInDetTrackData - Reconstructed Inner Detector Track
 *                   AMuonTrackData - Reconstructed Muon Detector Track
 *
 */
public abstract class ATrackData extends AData
{
    private static final ALogger logger = ALogger.getLogger(ATrackData.class);
    
    public static final int DRAW_HELIX = 0;
    public static final int DRAW_POLYLINE = 1;
    public static final int DRAW_SMOOTH = 2;
    public static final int DRAW_NEWHELIX = 3;
    public static final int ADD_HELIX_POINTS = 0;

    /** True if track residual is available */
    private boolean hasResidual = false;

    protected ATrackResidualData[] residuals = null;

    /** True if reconstructed track event data contains polyline information */
    protected boolean polylinedTrack = false;

    // these attributes are used to store polyline coordinates of tracks
    protected float x[][] = null;
    protected float y[][] = null;
    protected float z[][] = null;
    protected float rho[][] = null;
    protected float phi[][] = null;

    protected int numHelix[] = null;
    protected boolean cosmic[] = null;

    protected AHelix[] h = null;

    /**
     *  rhoVertex value - for STr provided in the event file, for reconstructed
     *  track (Track) it is calculated from first polyline coordinates.
     *  This value is tested in cutRhoVertexAfterInDetRadius() method.
     */
    protected float[] rhoVertex = null;

    protected int[] index = null;
    
    protected float chi2[] = null;
    protected int numDoF[] = null;
    private int[] vertexType = null;

    // methods --------------------------------------------------------------

    ATrackData(AHashMap p, AEvent e) throws AAtlantisException
    {
        super(p,e);

        String assocKey = getName() + getStoreGateKey();
        if (p.get("barcode") != null)
        {
            int[] barcode = p.getUnknownIntArray("barcode");
            AAssociation assoc = new AAssociation(assocKey, "STr", null, barcode, event);
            event.getAssociationManager().add(assoc);
        }
    	//Get vertex type numbers
        vertexType = p.getUnsureIntArray("vertexType"); 
        // if residual information is provided, second check is a workaround for a JiveXML bug -- EJ
        if(p.get("numTsos")!=null && p.get("tsosPullLoc1")!=null)
        {
            hasResidual = true;
            residuals = new ATrackResidualData[numData];
            int[] numTsos = p.getIntArray("numTsos");
            float[] tsosPullLoc1 = p.getFloatArray("tsosPullLoc1");
            float[] tsosResLoc1 = p.getFloatArray("tsosResLoc1");
            float[] tsosPullLoc2 = p.getFloatArray("tsosPullLoc2");
            float[] tsosResLoc2 = p.getFloatArray("tsosResLoc2");
            String[] tsosDetType = p.getUnsureStringArray("tsosDetType");

            int num = 0;
            for(int i=0; i<numData; i++)
            {
                int numPoints = numTsos[i];
                float[] pullLoc1 = new float[numPoints];
                float[] resLoc1 = new float[numPoints];
                float[] pullLoc2 = new float[numPoints];
                float[] resLoc2 = new float[numPoints];
                String[] detType = null;
                if(tsosDetType != null)
                    detType = new String[numPoints];
              
                for(int j=0; j<numPoints; j++)
                {
                    pullLoc1[j] = tsosPullLoc1[num];
                    resLoc1[j] = tsosResLoc1[num];
                    pullLoc2[j] = tsosPullLoc2[num];
                    resLoc2[j] = tsosResLoc2[num];
                    if(tsosDetType != null)
                        detType[j] = tsosDetType[num];
                    num++;
                }
                residuals[i] = new ATrackResidualData(this, i, numPoints, pullLoc1, 
                        resLoc1, pullLoc2, resLoc2, detType);
            }
        }
       
        // helix may come in different formats:
        // * helix track from reconstructed track ? 
        if (p.get("d0") != null)
        {
            h = new AHelix[numData];

            float[] d0 = p.getFloatArray("d0");
            float[] phi0 = p.getFloatArray("phi0");
            float[] tl = p.getFloatArray("cotTheta");
            float[] pt = p.getFloatArray("pt");
            float[] z0 = p.getFloatArray("z0");

            // this patch is useful when reading old event files generated by
            // some older version of JiveXML, in which cotTheta is set to inf by
            // error when eta equals to 0 for IDScan Tracks
            if ("TrigInDetTrack".equals((String) p.get("storeGateKey")))
            {
                for(int i = 0; i < tl.length; i++)
                {
                    if(Float.isInfinite(tl[i]))
                        tl[i] = 0.0f;
                }
            } // end of the patch

            // 2005-11-16 - numbers in event file in covMatrix too small, a lot
            // of them were 0.00000 hence multiplied by 10000 in JiveXML and
            // divided here by 10000. only Vertex fitter affected, didn't work
            // before anyway, so numbers are divided for all events
            if (p.get("covMatrix") != null)
            {
                float[][][] cov = new float[numData][5][5];
                float[] covMatrix = p.getFloatArray("covMatrix");
                // dividing numbers
                for (int i = 0; i < covMatrix.length; i++)
                {
                    covMatrix[i] = covMatrix[i] / 10000;
                }

                int n = 0;
                for (int i = 0; i < numData; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        for (int k = 0; k < j + 1; k++)
                        {
                            cov[i][j][k] = covMatrix[n++];
                            cov[i][k][j] = cov[i][j][k];
                        }
                    }

                    h[i] = new AHelix(d0[i], z0[i], (float) Math.toDegrees(phi0[i]), tl[i], pt[i], cov[i]);
                }
            }
            else
            {
                for (int i = 0; i < numData; i++) {
                    h[i] = new AHelix(d0[i], z0[i], (float) Math.toDegrees(phi0[i]), tl[i], pt[i]);
                }
            }
        }
        // * helix track from simulated charged tracks ?
        else if (p.get("rhoVertex") != null)
        {
            h = new AHelix[numData];

            // STr - save rhoVertex value for tests
            rhoVertex = p.getFloatArray("rhoVertex");
            float[] rhoEndVertex = p.getFloatArray("rhoEndVertex");
            float[] pt = p.getFloatArray("pt");
            float[] phi = p.getFloatArray("phi");
            float[] eta = p.getFloatArray("eta");
            float[] phiVertex = p.getFloatArray("phiVertex");
            float[] zVertex = p.getFloatArray("zVertex");
            // charge is calculated in ASTrData, based on code subtag
            // information and pdg.xml data
            int[] charge = p.getIntArray("charge");
            for (int i = 0; i < numData; i++)
            {
                //Create helix w/ end vertex information if rhoEndVertex is set
                if ( rhoEndVertex[i] != 0.0 )
                  h[i] = new AHelix(rhoVertex[i], phiVertex[i], zVertex[i], pt[i],
                                    (float)Math.toDegrees(phi[i]), eta[i], charge[i], rhoEndVertex[i]);
                //Otherwise create helix w/o end vertex information
                else 
                  h[i] = new AHelix(rhoVertex[i], phiVertex[i], zVertex[i], pt[i],
                                  (float)Math.toDegrees(phi[i]), eta[i], charge[i] );
            }
        }

        // if polyline information is provided for tracks
        if(p.get("numPolyline") != null)
        {
            int[] numPolyline = p.getIntArray("numPolyline");

            boolean hasPolyline = false;
            for (int i=0; i<numData; i++) {
                if (numPolyline[i] > 0) {
                    hasPolyline = true;
                    break;
                }
            }

            if (hasPolyline) {
                polylinedTrack = true;
                x = new float[numData][];
                y = new float[numData][];
                z = new float[numData][];
                rho = new float[numData][];
                phi = new float[numData][];
                rhoVertex = new float[numData];
                numHelix = new int[numData];
                cosmic = new boolean[numData];

                float[] polyX = p.getFloatArray("polylineX");
                float[] polyY = p.getFloatArray("polylineY");
                float[] polyZ = p.getFloatArray("polylineZ");
                int num = 0;
               
                for(int i = 0; i < numData; ++i)
                {
                    numHelix[i] = 0;
                    cosmic[i] = false;

                    // Do some checks on the track, decide what drawing we enable/disable for it.
                    if (numPolyline[i] >= 2) {
                        int first = num;
                        int next = num+1;

                        // Find the next different point on the track.
                        while (next < numPolyline[i]-1 && polyX[next]==polyX[first] && polyY[next]==polyY[first]) next++;

                        // Determine what direction the track has in rho and z.
                        float rStart = (float)Math.sqrt(polyX[first]*polyX[first] + polyY[first]*polyY[first]);
                        float rDir = (float)Math.sqrt(polyX[next]*polyX[next] + polyY[next]*polyY[next]) - rStart;
                        float zDir = Math.abs(polyZ[next]) - Math.abs(polyZ[first]);

                        // Now in order to extrapolate to the IP we the track to:
                        // - go radially outwards
                        // - go away from the IP in z
                        // - have perigee parameters
                        // - have its first polyline point some distance from the perigee
                        // - be an InDetTrack
                        if (rDir > 0 && zDir > 0 && h != null && h[i] != null && rStart-Math.abs(h[i].d0()) > 1
                            && this instanceof AInDetTrackData) {
                            numHelix[i] = ADD_HELIX_POINTS;
                        }

                        // Check if this is perhaps a cosmic. Cosmics are allowed
                        // to be drawn on both sides in the rho-z projection
                        if (rDir < 0) {
                            cosmic[i] = true;
                        }
                    } else if (h != null && h[i] != null && this instanceof AInDetTrackData) {

                        // In case of a track with only perigee parameters, draw it also as a helix.
                        numHelix[i] = ADD_HELIX_POINTS;
                    }
                    
                    x[i] = new float[numPolyline[i]+numHelix[i]];
                    y[i] = new float[numPolyline[i]+numHelix[i]];
                    z[i] = new float[numPolyline[i]+numHelix[i]];
                    rho[i] = new float[numPolyline[i]+numHelix[i]];
                    phi[i] = new float[numPolyline[i]+numHelix[i]];

                    for(int j = 0; j < numPolyline[i]; ++j)
                    {
                        x[i][j+numHelix[i]] = polyX[num];
                        y[i][j+numHelix[i]] = polyY[num];
                        z[i][j+numHelix[i]] = polyZ[num];
                        rho[i][j+numHelix[i]] = (float)Math.sqrt(polyX[num]*polyX[num]+polyY[num]*polyY[num]);
                        num++;
                        
                        if(j == 0)
                        {
                            // first coordinate of a track - calculate rhoVertex
                            rhoVertex[i] = (float) Math.sqrt( x[i][numHelix[i]] * x[i][numHelix[i]] +
                                                              y[i][numHelix[i]] * y[i][numHelix[i]] );
                        }
                    }

                    if (h != null && h[i] != null) {
                        h[i].setPoints(x[i], y[i], z[i], numPolyline[i]);
                    }
                    
                    // When perigee parameters are available we extend the track all the way down to the IP
                    if (numHelix[i] > 0) {
                        // Radius of curvature of this track in the magnetic field
                        double R = parameterStore.get("Event", "Curvature").getD() * Math.abs(h[i].pT());

                        // +1/-1 for a clockwise/anti-clockwise helix 
                        double S = AMath.getSign(h[i].pT());

                        // Coordinates of the center point for the helix
                        double xC = (S * h[i].d0() - R) * Math.cos(Math.toRadians(h[i].phi0()) + S * Math.PI / 2.);
                        double yC = (S * h[i].d0() - R) * Math.sin(Math.toRadians(h[i].phi0()) + S * Math.PI / 2.);

                        // Determine to what radius we have to draw this track based on its 
                        // perigee parameters. When space points are available for the track 
                        // we stop before the first space point and make a smooth connection.
                        // If no space points are available we draw all the way through the
                        // inner detector. Not any further though, because the track will stop 
                        // curving when it leaves the magnetic field.
                        double Rmax;
                        if (numPolyline[i] > 0) {
                            Rmax = Math.min(parameterStore.get("RTr", "RadiusTr").getD(),
                                Math.abs(Math.sqrt(x[i][numHelix[i]]*x[i][numHelix[i]]
                                    + y[i][numHelix[i]]*y[i][numHelix[i]]) - 1));
                        } else {
                            Rmax = parameterStore.get("RTr", "RadiusTr").getD();
                        }

                        // Calculate the phi value at which our helix intersects the maximum 
                        // radius determined above. We have one circle (detector) centered at 
                        // the origin with radius Rmax. The other circle (helix) we imagine to 
                        // be at x=d with a radius R. Now we can easily calculate the coordinates
                        // (xI,+/-yI) of the intersection points. The curving direction of the 
                        // helix determines which value of yI we have to use. Finally, the phiMax 
                        // we're interested in is the polar angle with respect to the center of the 
                        // helix at x=d. Calculation is straightforward. By adding phi0+S*PI/2 
                        // (as we do below) this solution is also valid for helices centered 
                        // around y!=0.
                        double d = Math.sqrt(xC * xC + yC * yC);
                        double xI = (Rmax * Rmax - R * R + d * d) / (2 * d);
                        double yI = Math.sqrt(Rmax * Rmax - xI * xI);
                        double phiMax = Math.atan2(S * yI, d - xI);

                        // This spreads the number of helix points evenly across the part of the 
                        // helix we're going to draw
                        double dphi = phiMax / numHelix[i];

                        for (int j=0; j<numHelix[i]; j++) {
                            // Points on the helix in user coordinates
                            x[i][j] = (float) (xC + R * Math.cos(Math.toRadians(h[i].phi0()) + S * Math.PI/2. - j * dphi));
                            y[i][j] = (float) (yC + R * Math.sin(Math.toRadians(h[i].phi0()) + S * Math.PI/2. - j * dphi));

                            // In R-Z the track is just a straight line
                            rho[i][j] = (float) Math.sqrt(x[i][j]*x[i][j] + y[i][j]*y[i][j]);
                            z[i][j] = (float) (h[i].z0() + h[i].cotanTheta() * (rho[i][j] - h[i].d0()));
                        }
                    }                
            
                    for (int j=0; j<numPolyline[i]+numHelix[i]; j++) {
                        phi[i][j] = (float)Math.atan2(y[i][j], x[i][j]);
                        
                        if (j>0 && Math.abs(phi[i][j-1]-phi[i][j]) > Math.PI) {
                            
                            // Don't let phi wrap around
                            if (phi[i][j] > phi[i][j-1]) {
                                phi[i][j] -= 2*Math.PI;
                            } else {
                                phi[i][j] += 2*Math.PI;
                            }
                        }
                    }
                }
            }
        } // if(p.get("numPolyline")

        if (h != null)
        {
            double[] phi = new double[numData];
            for (int i = 0; i < numData; i++)
                phi[i] = h[i].phi0();
            index = indexBy(phi);
        }
        else if(x != null && y != null)
        {
            double[] phi = new double[numData];
            for(int i = 0; i < numData; i++)
            {
                // if number of polylines for a track is 0 - Atlantis used
                // to crash here with IndexOutOfBoundException
                // check to avoid crashes with old events, from some point
                // JiveXML doesn't output tracks with number of polylines
                // coordinates less then 2
                if(x[i].length > 0 && y[i].length > 0)
                {
                    phi[i] = Math.toDegrees(Math.atan2(y[i][0], x[i][0]));
                }
                else
                {
                    String m  = getName() + ":" + getStoreGateKey() +
                                " datatype rejected\n" +
                                "(no polyline coordinates).";
                    throw new AAtlantisException(m, false); // non fatal
                }
            }
            index = indexBy(phi);            
        }

        if (p.get("chi2") != null) chi2 = p.getFloatArray("chi2");
        if (p.get("numDoF") != null) numDoF = p.getIntArray("numDoF");
    } // ATrackData() -------------------------------------------------------


    public float[][] getX()
    {
        return x;
    }

    public float[][] getY()
    {
        return y;
    }

    public float[][] getZ()
    {
        return z;
    }

    public float[][] getRho()
    {
        return rho;
    }

    public float[][] getPhi()
    {
        return phi;
    }

    public boolean getResidualStatus()
    {
        return hasResidual;
    }

    public ATrackResidualData getTrackResidual(int index)
    {
        return residuals[index];
    }

    // moved from the former iPatData class where it was without the condition
    protected void calculateRhoPhi()
    {
        if(!polylinedTrack) {
            super.calculateRhoPhi();
        }
    } // calculateRhoPhi() --------------------------------------------------



    protected void applyCuts()
    {
        cutIndex();
        if (h != null)
        {
            cut("CutsInDet", "z0-zVtx", " |z0-zVtx|", getZ0Primary());
            cut("CutsInDet", "z0", " |z0|", getZ0());
            cut("CutsInDet", "Pt", " |Pt|", getPt());
            cut("CutsInDet", "d0", " |d0|", getD0());
            cut("CutsInDet", "d0Loose", " |d0Loose|", getD0());
        }
        cutPhi();
        cutEta();
        cutNextInDrawList();
        cutReconstructedVertex();     
    } // applyCuts() --------------------------------------------------------



    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+
        ((h!=null)?h[index].toString():"");
    	
        
        String msg = getNameScreenName() + " (id: " + id[index] + " index: " + index + ")";
        msg += "\n storegate key: ";
        msg += (storeGateKey == null ? "n/a" : storeGateKey);
        if (h != null)
            msg += h[index].toString();
        if (chi2 != null && numDoF != null)
            msg += "\n chi2/numDoF = " + chi2[index]/numDoF[index];
        return msg;

    } // getHitInfo() -------------------------------------------------------


    // all get**User() methods which are only called when tracks are drawn
    // as polylines
    protected ACoord getYXUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int numPoints = x[list].length;
            hv[0][i] = new double[numPoints];
            hv[1][i] = new double[numPoints];
            for(int j = 0; j < numPoints; j++)
            {
                hv[0][i][j] = x[list][j];
                hv[1][i][j] = y[list][j];
            }
            index[i] = list;
        }

        if (parameterStore.get(PARAMETER_GROUP, "DrawnAs").getI() == DRAW_SMOOTH)
            return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES);
        else
            return new ACoord(hv, index, this, ACoord.POLYLINES);

    } // getYXUser() --------------------------------------------------------



    protected ACoord getFRUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int numPoints = x[list].length;
            int skip = 0;
            
            // Skip points with very small R, phi changes rapidly here
            for (skip=0; skip<numPoints; skip++)
                if (rho[list][skip] > 2.) break;
            
            hv[0][i] = new double[numPoints-skip];
            hv[1][i] = new double[numPoints-skip];
            
            double phiStart = 0.;
            if(numPoints > 0) phiStart = phi[list][0];
            for(int j = skip; j < numPoints; j++)
            {
                double phiTemp = phi[list][j];
                if(phiTemp - phiStart > Math.PI) phiTemp -= AMath.TWO_PI;
                if(phiTemp - phiStart < -Math.PI) phiTemp += AMath.TWO_PI;
                
                hv[0][i][j-skip] = rho[list][j];
                hv[1][i][j-skip] = Math.toDegrees(phiTemp);
            }
            index[i] = list;
        }

        if (parameterStore.get(PARAMETER_GROUP, "DrawnAs").getI() == DRAW_SMOOTH)
            return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES).includePhiWrapAround("FR");
        else
            return new ACoord(hv, index, this, ACoord.POLYLINES).includePhiWrapAround("FR");

    } // getFRUser() --------------------------------------------------------



    protected ACoord getFZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int numPoints = x[list].length;
            int skip = 0;
            
            // Skip points with very small R, phi changes rapidly here
            for (skip=0; skip<numPoints; skip++)
                if (rho[list][skip] > 2.) break;
            
            hv[0][i] = new double[numPoints-skip];
            hv[1][i] = new double[numPoints-skip];
            // treat phi wraparound
            double phiStart = 0.;
            if(numPoints > 0) phiStart = phi[list][0];
            for(int j = skip; j < numPoints; j++)
            {
                hv[0][i][j-skip] = z[list][j];
                double phiTemp = phi[list][j];
                if(phiTemp - phiStart > Math.PI) phiTemp -= AMath.TWO_PI;
                if(phiTemp - phiStart < -Math.PI) phiTemp += AMath.TWO_PI;
                hv[1][i][j-skip] = Math.toDegrees(phiTemp);
            }
            index[i] = list;
        }

        if (parameterStore.get(PARAMETER_GROUP, "DrawnAs").getI() == DRAW_SMOOTH)
            return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES).includePhiWrapAround("FZ");
        else
            return new ACoord(hv, index, this, ACoord.POLYLINES).includePhiWrapAround("FZ");

    } // getFZUser() --------------------------------------------------------



    protected ACoord getRZUser()
    {
        // not quite correct need to split tracks crossing phi boundary
        makeDrawList();
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int numPoints = rho[list].length;
            hv[0][i] = new double[numPoints];
            hv[1][i] = new double[numPoints];

            if (numPoints > 0) {
                int sign = AParameterUtilities.getRhoSign(x[list][numPoints-1], y[list][numPoints-1]);
                for(int j = 0; j < numPoints; j++)
                {
                    if (cosmic != null && cosmic[i]) {
                        sign = AParameterUtilities.getRhoSign(x[list][j], y[list][j]);
                    }
                    hv[0][i][j] = z[list][j];
                    hv[1][i][j] = rho[list][j] * sign;
                }
            }
            index[i] = list;
        }

        if (parameterStore.get(PARAMETER_GROUP, "DrawnAs").getI() == DRAW_SMOOTH)
            return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES);
        else
            return new ACoord(hv, index, this, ACoord.POLYLINES);

    } // getRZUser() --------------------------------------------------------



    protected ACoord getXZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        double phi0 = Math.toRadians(AProjectionXZ.getPhi());
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int numPoints = rho[list].length;
            hv[0][i] = new double[numPoints];
            hv[1][i] = new double[numPoints];
            for(int j = 0; j < numPoints; j++)
            {
                hv[0][i][j] = z[list][j];
                hv[1][i][j] = rho[list][j] * Math.cos(phi[list][j] - phi0);
            }
            index[i] = list;
        }

        if (parameterStore.get(PARAMETER_GROUP, "DrawnAs").getI() == DRAW_SMOOTH)
            return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES);
        else
            return new ACoord(hv, index, this, ACoord.POLYLINES);

    } // getXZUser() --------------------------------------------------------

    protected ACoord getYZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        double phi0 = Math.toRadians(AProjectionXZ.getPhi());
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int numPoints = rho[list].length;
            hv[0][i] = new double[numPoints];
            hv[1][i] = new double[numPoints];
            for(int j = 0; j < numPoints; j++)
            {
                hv[0][i][j] = z[list][j];
                hv[1][i][j] = rho[list][j] * Math.sin(phi[list][j] - phi0);
            }
            index[i] = list;
        }

        if (parameterStore.get(PARAMETER_GROUP, "DrawnAs").getI() == DRAW_SMOOTH)
            return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES);
        else
            return new ACoord(hv, index, this, ACoord.POLYLINES);

    } // getYZUser() --------------------------------------------------------



    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
        if(h != null && (projection instanceof AProjectionVP || projection instanceof AProjection3D))
        {
            // in VPlot only helices are drawn, so try to draw tracks as helices
            // in VPlot by default ignoring Track->DrawAs option for VPlot
            drawHelix(window, ag, projection);
        }
        else
        {
            AParameter drawPar = parameterStore.get(PARAMETER_GROUP, "DrawnAs");
            int drawnAs = drawPar != null ? drawPar.getI() : DRAW_HELIX;
            if (polylinedTrack && (h == null || drawnAs == DRAW_POLYLINE || drawnAs == DRAW_SMOOTH)) 
            {
                // draw as polyline
                ag.draw(window.calculateDisplay(getUser(projection)));
            } 
            else if (h != null)
            {
                // draw as helix
                drawHelix(window, ag, projection);
            }
        }

    } // draw() -------------------------------------------------------------

    protected void cutRhoVertexAfterInDetRadius()
    {
        int num = 0;
        int wrong = 0;
        int list = 0;
        double rho = 108; // 1.08m
        double parRhoTr = parameterStore.get("RTr", "RadiusTr").getD();
        for(int i = 0; i < numDraw; i++)
        {
            list = listdl[i];
            if(this.rhoVertex[list] > Math.min(rho, parRhoTr))
            {
                wrong++;
            }
            else
            {
                // include into drawlist
                listdl[num++] = list;
            }
        }

        if(wrong > 0)
        {
            logger.debug("AHelix.cutRhoVertexAfterInDetRadius()");
            String key = this.getStoreGateKey();
            key = key != null ? ":" + key : "";
            logger.debug("  " + numDraw + " " + this.getName() + key +
                          " before test, " + wrong + " removed\n");
        }
        numDraw = num;
    } // cutRhoVertexAfterInDetRadius() -------------------------------------

    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        switch (colorFunction)
        {
            case 0:
                colorByConstant();
                break;
            case 1:
                colorByIndex(index);
                break;
            case 2:
                colorBy(getPt());
                break;
            case 3:
                colorByCollection();
                break;
            case 4:
                colorBy("STr");
                break;
            case 5:               
                colorBy(getRVx());
                break;
            case 6:              
                colorBy(getJets());
                break;
            case 7: // by objects
                colorByObjects();
                break;
        }

        return 7;
    }

    public int[] getDrawList()
    {
        int[] temp = new int[numDraw];
        System.arraycopy(listdl, 0, temp, 0, numDraw);
        return temp;
    }

    protected void cutNextInDrawList()
    {
        AParameter nextTrkPar = parameterStore.get(PARAMETER_GROUP, "NextTrack");
        if(!nextTrkPar.getStatus())
        {
            return;
        }
        if(numDraw == 0)
        {
            return;
        }

        int nextTrk = nextTrkPar.getI() % numData;
        int nextTrkIndex = nextTrk;
        boolean hasFound = false;
        while (!hasFound)
        {
            for (int i=0; i<numDraw; ++i)
            {
                if(listdl[i] == nextTrk)
                {
                    nextTrkIndex = nextTrk;
                    hasFound = true;
                }
            }
            if(!hasFound)
                nextTrk = (nextTrk + 1) % numData;
        }
        nextTrkPar.setI(nextTrkIndex);
        numDraw = 1;
        listdl[0] = nextTrkIndex;
    }
   
    protected void cutPhi()
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutPhi");
        boolean usePhiCut = par.getStatus();

        if (usePhiCut)
        {
            // use degrees for tracks
            double phiCut = par.getD();
            double phiMid = AMath.nearestPhiDegrees(parameterStore.get("CutsATLAS", "PhiMiddle").getD());
            // use vplot as projection
            AProjection2D projection = (AProjection2D) AProjectionsManager.getProjection("VP");

            int num = 0;
            for (int i = 0; i < numDraw; i++)
            {
                int list = listdl[i];
                ADHelix dH = new ADHelix(h[list]);
                double s1 = dH.getAStart();
                double s2 = dH.getAEnd();
                s1 = dH.intersectWithRadialCylinder(projection.getMinRho(), s1, s2);
                double sEnd = dH.intersectWithCylinder(true, AProjectionVP.getRhoVPlot(), true, AProjectionVP.getZVPlot());
                s2 = Math.max(Math.min(s2, sEnd), s1);

                double phiDiffStart = Math.abs(AMath.nearestPhiDegrees(dH.getPhi(s1), phiMid) - phiMid);
                double phiDiffEnd = Math.abs(AMath.nearestPhiDegrees(dH.getPhi(s2), phiMid) - phiMid);
                double phiDiffMiddle = Math.abs(AMath.nearestPhiDegrees(dH.getPhi((s1 + s2) / 2), phiMid) - phiMid);
                // treats wrap around
                int numPointsInside = 0;

                if (phiDiffStart < phiCut)
                    numPointsInside++;
                if (phiDiffMiddle < phiCut)
                    numPointsInside++;
                if (phiDiffEnd < phiCut)
                    numPointsInside++;
                if (numPointsInside > 1)
                    listdl[num++] = list;
            }
            numDraw = num;
        }
    }

    /**
     * Cut tracks if connected/unconnected to reconstructed vertices.
     */
    protected void cutReconstructedVertex()
    {
        int cutOption = parameterStore.get("CutsObjects", "RTrsByRVtx").getI();
        cutByAssociationTo( "RVx", getReconstructedTracks(), cutOption);
    }
     
    
    protected void cutEta()
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");

        if (par.getStatus())
        {
            // use vplot as projection
            AProjection2D projection = (AProjection2D) AProjectionsManager.getProjection("VP");

            double etaCut = par.getD();
            double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
            double etaC1 = etaMid - etaCut;
            double etaC2 = etaMid + etaCut;
            int num = 0;
            for (int i = 0; i < numDraw; i++)
            {
                int list = listdl[i];
                ADHelix dH = new ADHelix(h[list]);
                double s1 = dH.getAStart();
                double s2 = dH.getAEnd();
                s1 = dH.intersectWithRadialCylinder(projection.getMinRho(), s1, s2);
                double sEnd = dH.intersectWithCylinder(true, AProjectionVP.getRhoVPlot(), true, AProjectionVP.getZVPlot());
                s2 = Math.max(Math.min(s2, sEnd), s1);
                double[] etaPoints = new double[] { dH.getEta(s1), dH.getEta((s1 + s2) / 2), dH.getEta(s2) };

                int numPointsInside = 0;
                for (int j = 0; j < etaPoints.length; ++j)
                    if (etaC1 < etaPoints[j] && etaPoints[j] < etaC2)
                        numPointsInside++;
                if (numPointsInside > 1)
                    listdl[num++] = list;
            }
            numDraw = num;
        }
    }

    // does not work correctly if primary vertex is displaced in YX plane
    private float[] getZ0Primary()
    {
        double[] pVtx = event.getPrimaryVertex();
        float[] temp = new float[numData];
        double zVertex = pVtx[2];
        for (int i = 0; i < numDraw; i++)
            temp[listdl[i]] = (float) (h[listdl[i]].z0() - zVertex);
        return temp;
    }

    private float[] getZ0()
    {
        float[] temp = new float[numData];
        for (int i = 0; i < numDraw; i++)
            temp[listdl[i]] = (float) h[listdl[i]].z0();
        return temp;
    }

    protected float[] getPt()
    {
        float[] temp = new float[numData];
        for (int i = 0; i < numDraw; i++)
            temp[listdl[i]] = (float) h[listdl[i]].pT();
        return temp;
    }


    // d0 is calculated w.r.t. to XY position of the primary vertex
    private float[] getD0()
    {
        double[] pVtx = event.getPrimaryVertex();
        float[] temp = new float[numData];
        for (int i = 0; i < numDraw; i++)
        {
            double phi0 = Math.toRadians(h[listdl[i]].phi0());

            temp[listdl[i]] = (float) (-h[listdl[i]].d0() + pVtx[1] * Math.cos(phi0) - pVtx[0] * Math.sin(phi0));

        }
        return temp;
    }


    // give back Drawable helices
    public ADHelix[] getHelices()
    {
        makeDrawList();
        ADHelix[] tempList = new ADHelix[numDraw];

        for (int i = 0; i < numDraw; i++)
            if (h != null && h[listdl[i]] != null)
            {
                tempList[i] = new ADHelix(h[listdl[i]]);
                if (tempList[i].getAStart() == tempList[i].getAEnd())
                {
                    tempList[i] = null;
                }
            }
            else
                tempList[i] = null;
        return tempList;
    }

    // used by vertex package when changing the position on the helix from which
    // to start drawing
    public AHelix getModifiableHelix(int i)
    {
        if (h != null)
            return h[i];
        else
            return null;
    }

    // info on tracks contained in vplot rubberband
    public String getVPHitInfo()
    {
        makeDrawList();
        if (numDraw == 0)
            return "";
        double sumP = 0.;
        double sumPt = 0.;

        for (int i = 0; i < numDraw; ++i)
        {
            sumPt += Math.abs(h[listdl[i]].pT());
            sumP += AMath.getPFromPttL(h[listdl[i]].pT(), h[listdl[i]].cotanTheta());
        }

        String msg = numDraw + " " + getFullName();
        msg += "  sum(PT) = " + String.format("%.1f",sumPt) + "  sum(P) = " + String.format("%.1f",sumP);
        return msg;
    }

    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        for (int i = 0; i < num; ++i) {
            if (h == null || h[list[i]] == null) continue;
            // Use the pion mass (~140 MeV) here
            sum.add(new A4Vector(h[list[i]].p(), 0.14));
        }
        return sum;
    }
    
    public A4Vector get4Vector(int num, int[] list, double mass)
    {
        A4Vector sum = new A4Vector();
        for (int i = 0; i < num; ++i) {
            if (h == null || h[list[i]] == null) continue;
            sum.add(new A4Vector(h[list[i]].p(), mass));
            
              
        }
        return sum;
    }

    public void zoomAroundTracks()
    {
        AWindow window = ACanvas.getCanvas().getCurrentWindow();
        AProjection projection = window.getProjection();
        if (projection instanceof AProjectionLegoPlot)
        {
            AOutput.append("Zoom Next Track unavailable for LegoPlot\n", ALogInterface.NORMAL);
        }
        else if (projection instanceof AProjection2D)
        {
            ACoord user;
            if (projection instanceof AProjectionVP)
                user = window.calculateUser(getVPDisplayHelices(window, (AProjection2D) projection));
            else if (polylinedTrack)
                user = getUser((AProjection2D) projection);
            else
                user = window.calculateUser(getDisplayHelices(window, (AProjection2D) projection));
            double[] min = new double[2];
            double[] max = new double[2];
            min[0] = min[1] = 100000.;
            max[0] = max[1] = -100000.;
            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < user.hv[i].length; ++j)
                    for (int k = 0; k < user.hv[i][j].length; ++k)
                    {
                        if (user.hv[i][j][k] < min[i])
                            min[i] = user.hv[i][j][k];
                        if (user.hv[i][j][k] > max[i])
                            max[i] = user.hv[i][j][k];
                    }
            // increase by 10% to improve appearence
            for (int i = 0; i < 2; ++i)
            {
                double diff = (max[i] - min[i]) / 2.;
                double middle = (max[i] + min[i]) / 2.;
                if (diff > 0.)
                {
                    max[i] = middle + 1.1 * diff;
                    min[i] = middle - 1.1 * diff;
                }
            }
            // ensure entire trt hits included
            if (projection instanceof AProjectionPhi)
            {
                min[1] -= 0.5;
                max[1] += 0.5;
            }

            if (max[0] - min[0] > 0 && max[1] - min[1] > 0)
            {
                window.setUserCorners(min[0], max[0], min[1], max[1]);
            }
        }
    }

    public void drawHelix(AWindow window, AGraphics ag, AProjection2D projection)
    {
        if (projection instanceof AProjectionVP)
        {
            // phi wrap around is done in user coordinates, so we convert to user coordinates,
            // do the phi wrap around and convert back to display coordinates
            ag.draw(window.calculateDisplay(window.calculateUser(
                    getVPDisplayHelices(window, projection)).includePhiWrapAround(projection.getName())
            ));
        }
        else if (projection instanceof AProjectionRZ)
        {
            ag.draw(getRZDisplayHelices(window, projection));
        }
        else if (projection instanceof AProjectionPhi)
        {
            // phi wrap around in user coordinates, see above
            ag.draw(window.calculateDisplay(window.calculateUser(
                    getDisplayHelices(window, projection)).includePhiWrapAround(projection.getName())
            ));

        }
        else
        {
            ag.draw(getDisplayHelices(window, projection));
        }
    }

    // treat discontinuity when RZ sign changes crossing the phi boundary
    private ACoord getRZDisplayHelices(AWindow window, AProjection2D projection)
    {
        ACoord display = getDisplayHelices(window, projection);
        ACoord user = projection.inverseNonLinearTransform(window.calculateUser(display));

        double[][] rho = user.hv[1];

        int extraTrackSegments = 0;

        for (int i = 0; i < rho.length; i++)
            for (int j = 1; j < rho[i].length - 2; j++)
                if (rho[i][j] * rho[i][j + 1] < 0. && Math.abs(rho[i][j]) > 2.)
                    extraTrackSegments++;

        double hv[][][] = new double[2][rho.length + extraTrackSegments][];
        int index[] = new int[rho.length + extraTrackSegments];

        extraTrackSegments = 0;
        for (int i = 0; i < rho.length; i++)
        {
            int startOfSegment = 0;
            int endOfLastSegment = rho[i].length - 1;

            for (int j = 0; j < rho[i].length - 1; j++)
                if (rho[i][j] * rho[i][j + 1] < 0. && Math.abs(rho[i][j]) > 2.)
                {
                    if (j == 0)
                    {
                        if (Math.abs(rho[i][0]) > 1.)
                            startOfSegment = 1;
                    }
                    else if (j == rho[i].length - 2)
                        endOfLastSegment = rho[i].length - 2;
                    else
                    {
                        for (int k = 0; k < hv.length; ++k)
                        {
                            hv[k][i + extraTrackSegments] = new double[j + 1 - startOfSegment];
                            System.arraycopy(user.hv[k][i], startOfSegment, hv[k][i + extraTrackSegments], 0, j + 1 - startOfSegment);
                        }
                        index[i + extraTrackSegments] = user.index[i];
                        startOfSegment = j + 1;
                        extraTrackSegments++;
                    }
                }
            if (startOfSegment == 0 && endOfLastSegment == rho[i].length - 1)
            {
                for (int k = 0; k < hv.length; ++k)
                    hv[k][i + extraTrackSegments] = user.hv[k][i];
                index[i + extraTrackSegments] = user.index[i];
            }
            else
            {
                for (int k = 0; k < hv.length; ++k)
                {
                    hv[k][i + extraTrackSegments] = new double[endOfLastSegment - startOfSegment + 1];
                    System.arraycopy(user.hv[k][i], startOfSegment, hv[k][i + extraTrackSegments], 0, endOfLastSegment - startOfSegment + 1);
                }
                index[i + extraTrackSegments] = user.index[i];
            }
        }

        rho = hv[1];
        for (int i = 0; i < rho.length; i++)
        {
            int j = 0;

            while (j < rho[i].length && Math.abs(rho[i][j]) < 2.)
                j++;
            if (j < rho[i].length)
                if (rho[i][j] > 0.)
                    for (int k = 0; k < j; k++)
                        rho[i][k] = Math.abs(rho[i][k]);
                else
                    for (int k = 0; k < j; k++)
                        rho[i][k] = -Math.abs(rho[i][k]);
        }

        return window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv, index, this, ACoord.POLYLINES)));
    }

    protected ACoord getDisplayHelices(AWindow window, AProjection2D projection)
    {
        AParameter drawPar = parameterStore.get(PARAMETER_GROUP, "DrawnAs");
        int drawnAs = drawPar != null ? drawPar.getI() : DRAW_HELIX;
        
        if (drawnAs == DRAW_HELIX) {
            ADHelix[] dhelix = getHelices();
            double[][][] hv = new double[2][dhelix.length][0];
            int[] index = getDrawList();

            for (int j = 0; j < dhelix.length; ++j) {
                if (dhelix[j] != null) {
                    double s1 = dhelix[j].getAStart(); // Get start phi-angle
                    double s2 = dhelix[j].getAEnd(); // Get end phi-angle

                    // s1 becomes phi at the intersection of the minimal radius
                    s1 = dhelix[j].intersectWithRadialCylinder(projection.getMinRho(), s1, s2); // minrho = 0
                    // avoid drawing discontinuous in phi
                    dhelix[j].setPhiStart(s1, s2);
                    // returning a set of points to be drawn for this helix
                    ACoord pointsOnHelix = dhelix[j].drawHelix(window, projection, s1, s2);

                    hv[0][j] = pointsOnHelix.hv[0][0];
                    hv[1][j] = pointsOnHelix.hv[1][0];
                }
            }
            return new ACoord(hv, index, this, ACoord.POLYLINES);
        } else { // DRAW_NEW_HELIX
            
            // V-plot needs some additional settings
            boolean showS3D = parameterStore.get("Data", "S3D").getStatus();
            AParameter shortV = parameterStore.get("VP", "ShortV");
            
            makeDrawList();
            double[][][] hv = new double[2][numDraw][];
            int[] index = new int[numDraw];
            
            for (int i=0; i<numDraw; i++) {
                int j = listdl[i];
                index[i] = j;
                
                if (projection instanceof AProjectionYX) {

                    hv[0][i] = h[j].getX(h[j].getAlphaMin(), h[j].getAlphaMax());
                    hv[1][i] = h[j].getY(h[j].getAlphaMin(), h[j].getAlphaMax());
                    
                } else if (projection instanceof AProjectionRZ 
                            || projection instanceof AProjectionXZ
                            || projection instanceof AProjectionYZ) {

                    double alphaMin = h[j].getAlphaCylinder(projection.getMinRho(), AHelix.TRACKER_LENGTH);                    
                    hv[0][i] = h[j].getZ(alphaMin, h[j].getAlphaMax());
                    hv[1][i] = h[j].getRho(alphaMin, h[j].getAlphaMax(), true);
                    
                    if (projection instanceof AProjectionXZ || projection instanceof AProjectionYZ) {
                        double phiC = AProjectionXZ.getPhi();                        
                        if (projection instanceof AProjectionYZ) phiC += 90;
                        
                        double phi[] = h[j].getPhi(alphaMin, h[j].getAlphaMax(), false);
                        for (int k=0; k<hv[1][i].length; k++) {
                            hv[1][i][k] = Math.abs(hv[1][i][k]) * Math.cos(Math.toRadians(phi[k] - phiC));
                        }
                    }
                } else if (projection instanceof AProjectionFR) {
                    
                    double alphaMin = h[j].getAlphaCylinder(projection.getMinRho(), AHelix.TRACKER_LENGTH);
                    hv[0][i] = h[j].getRho(alphaMin, h[j].getAlphaMax(), false);
                    hv[1][i] = h[j].getPhi(alphaMin, h[j].getAlphaMax(), false);
                    
                } else if (projection instanceof AProjectionFZ) {
                    
                    double alphaMin = h[j].getAlphaCylinder(projection.getMinRho(), AHelix.TRACKER_LENGTH);
                    hv[0][i] = h[j].getZ(alphaMin, h[j].getAlphaMax());
                    hv[1][i] = h[j].getPhi(alphaMin, h[j].getAlphaMax(), false);
                    
                } else if (projection instanceof AProjectionVP) {
                    
                    double alphaMin = h[j].getAlphaCylinder(projection.getMinRho(), AHelix.TRACKER_LENGTH);
                    double alphaMax = h[j].getAlphaExtrapolated(AProjectionVP.getRhoVPlot(), AHelix.TRACKER_LENGTH); 
                    if (shortV.getStatus() && !showS3D) {
                        alphaMin = alphaMax - shortV.getD() * (alphaMax - alphaMin);
                    }
                    
                    hv[0][i] = h[j].getEta(alphaMin, alphaMax);
                    hv[1][i] = h[j].getPhi(alphaMin, alphaMax, true);
                    
                }
            }
            
            return window.calculateDisplay(projection.nonLinearTransform(new ACoord(hv, index, this, ACoord.POLYLINES)));
        }
    }

    // a track has two arms in the VPlot
    protected ACoord getVPDisplayHelices(AWindow window, AProjection2D projection)
    {
        boolean drawApex = parameterStore.get("VP", "DrawApex").getStatus();
        AParameter drawPar = parameterStore.get(PARAMETER_GROUP, "DrawnAs");
        int drawnAs = drawPar != null ? drawPar.getI() : DRAW_HELIX;
        
        if (drawnAs == DRAW_HELIX) {
            ADHelix[] dhelix = getHelices();
            int size = 2 * dhelix.length;
            if (drawApex)
                size = 3 * dhelix.length;
            double[][][] hv = new double[2][size][0];
            int[] index = new int[size];
            int[] indexIn = getDrawList();
            int num = 0;

            for (int j = 0; j < dhelix.length; ++j)
                if (dhelix[j] != null)
                {
                    double s1 = dhelix[j].getAStart();
                    double s2 = dhelix[j].getAEnd();

                    int mode = parameterStore.get("VP", "Mode").getI();
                    if(mode <= AProjectionVP.MODE_HCAL_LAYER_3
                        && mode >= AProjectionVP.MODE_ECAL_LAYER_0) {
                      s2 = 179;
                    }

                    s1 = dhelix[j].intersectWithRadialCylinder(projection.getMinRho(), s1, s2);
                    double sEnd = dhelix[j].intersectWithCylinder(true, AProjectionVP.getRhoVPlot(), true, AProjectionVP.getZVPlot());

                    s2 = Math.max(Math.min(s2, sEnd), s1);
                    // if the whole helix is to be drawn (which are unusual
                    // helices, shorten it a little to avoid wraparound problems
                    if (s1 == 0. && s2 == 180.)
                        s2 = 179.;
                    if (parameterStore.get("VP", "ShortV").getStatus() && !parameterStore.get("Data", "S3D").getStatus())
                        s1 = s2 - parameterStore.get("VP", "ShortV").getD() * (s2 - s1);
                    if (s2 > s1)
                    {
                        int signMin = -1;
                        int signMax = 1;
                        double h = 0;
                        double v = 0;
                        for (int sign = signMin; sign <= signMax; sign += 2)
                        {
                            // ugly must change structure at some point
                            AProjectionVP.sign = sign;
                            ACoord pointsOnHelix = dhelix[j].drawHelix(window, projection, s1, s2);
                            hv[0][num] = pointsOnHelix.hv[0][0];
                            hv[1][num] = pointsOnHelix.hv[1][0];
                            index[num] = indexIn[j];
                            h = hv[0][num][hv[0][num].length - 1];
                            v = hv[1][num][hv[0][num].length - 1];
                            num++;
                        }
                        if (drawApex)
                        {
                            int a = 3;
                            int b = 7;
                            hv[0][num] = new double[] { h - a, h + a, h, h, h - a, h + a };
                            hv[1][num] = new double[] { v - b, v - b, v - b, v + b, v + b, v + b };
                            index[num] = indexIn[j];
                            num++;
                        }
                    }
                }
            return new ACoord(hv, index, this, ACoord.POLYLINES);
        } else { // DRAW_NEW_HELIX is handled by the normal method
            return getDisplayHelices(window, projection);
        }
    }
}

