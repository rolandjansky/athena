package atlantis.data;

import java.awt.event.ActionEvent;
import java.awt.geom.Point2D;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import java.util.Vector;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.tree.DefaultMutableTreeNode;

import atlantis.canvas.ACanvas;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.colormap.AColorMap;
import atlantis.list.AList;
import atlantis.list.AListManager;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AEnumeratorParameter;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.AMath;


/**
 *
 * Transition Radiation Tracker data
 *
 * data from the event file
 * id - processed by AData
 * barcode (old) / barcodes and numBarcodes (new) - processed by AHitData
 * phi
 * rhoz - rho (for barrel) or z (for endcap)
 * driftR - drift radius
 * threshold
 * sub - subdetector
 *
 * still need to have correct phi calculated from pVtx for cuts
 * still need to have correct rho calculated from pVtx for cuts
 *
 */
public class ATRTData extends AHitData
{
    protected float[] phi = null;
    protected float[] rhoz = null;
    protected float[] driftR = null;
    protected int[] threshold = null;
    protected int[] noise = null;
    // bitPattern subtag information implemented in the retriever but
    // currently (2009-01-21) not used or necessary but
    // said to may become useful later
    protected int[] bitPattern = null;
    protected float[] timeOverThreshold = null;
    protected HashMap<String, int[]> driftSign = new HashMap<String, int[]>();
    protected HashMap<String, int[]> isOutlier = new HashMap<String, int[]>();
    //error code for when driftSign/isOutlier is not present
    private static final int NO_DATA =-99;



    public String getParameterGroup()
    {
        return "TRT";
    }


    public String getName()
    {
        return "TRT";
    }


    public String getNameScreenName()
    {
        return "TRT_DriftCircle";
    }


    public int getDriftSign(int index)
    {
        //find current selection of track
        AEnumeratorParameter listBox = (AEnumeratorParameter) parameterStore.get("InDetTrack", "InDetTrackCollections");
        String currentSelection = listBox.getCurrentText();
        if(driftSign!=null && !currentSelection.equals("All"))
        {
            //find data from current track
            int[] data = (int[]) driftSign.get(currentSelection);
            if(data!=null)
                return data[index];
        }
        //no present so return error code
        return NO_DATA;
    }


    public int getIsOutlier(int index)
    {
        //find current selection of track
        AEnumeratorParameter listBox = (AEnumeratorParameter) parameterStore.get("InDetTrack", "InDetTrackCollections");
        String currentSelection = listBox.getCurrentText();
        if(isOutlier!=null && !currentSelection.equals("All"))
        {
            //find data from current track
            int[] data = (int[]) isOutlier.get(currentSelection);
            if(data!=null)
                return data[index];
        }
        //no present so return error code
        return NO_DATA;
    }

    
    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+"\n"+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        
        StringBuilder s = new StringBuilder();
        String[] decodedId = AIdHelper.getFullIdentifier(id[index]);
        s.append(getNameScreenName() + " (id: " + decodedId[0] +
            " index: " + index + ")");
        // iterate over decodedId to get further details (if available),
        // first item [0] was already printed out above
        for(int i = 1; i < decodedId.length; i++)
        {
            s.append("\n " + decodedId[i]);
        }
        if(sub[index] == 1 || sub[index] == 2)
        {
            s.append("\n " + AMath.RHO + " = " + String.format("%.1f",rhoz[index]) + " cm");
        }
        else
        {
            s.append("\n z = " + String.format("%.1f",rhoz[index]) + " cm");
        }
        s.append("\n " + AMath.PHI + " = " +
        String.format("%.1f",Math.toDegrees(phi[index])) + AMath.DEGREES +
        "  (" + String.format("%.3f",phi[index]) + " rad)" +
        "\n drift radius = " + String.format("%.3f",driftR[index]) + " cm" +
        "\n threshold = " + threshold[index] +
        "\n sub = " + sub[index]);

        s.append(super.getHitInfo(index)); // finds barcode information

        int d = getDriftSign(index);
        s.append((d!=NO_DATA) ? ("\n driftSign = " + d) : "\n driftSign = n/a");
        int o = getIsOutlier(index);
        s.append((o!=NO_DATA) ? ("\n isOutlier = " + o) : "\n isOutlier = n/a");
        s.append("\n noise = " + (noise != null ? noise[index] : "n/a"));
        s.append("\n time over threshold = " +
             (timeOverThreshold != null ? timeOverThreshold[index] : "n/a"));

        return s.toString();

    } // getHitInfo() -------------------------------------------------------


    public ATRTData(AHashMap p, AEvent e)
    {
        super(p,e);
        phi = p.getFloatArray("phi");
        rhoz = p.getFloatArray("rhoz");
        driftR = p.getFloatArray("driftR");
        threshold = p.getUnknownIntArray("threshold");
        noise = p.getUnsureIntArray("noise");
        if(p.getIntArray("sub") == null)
        {
            for(int i=0; i<numData; i++)
            {
                sub[i] = getSub(id[i]);
            }
        }
        // the way to retrieve bitPattern subtag information
        // currently (2009-01-21) not used or necessary but
        // said to may become useful later
        // bitPattern = p.getIntArray("bitPattern");
        timeOverThreshold = p.getFloatArray("timeOverThreshold");
        
    } // ATRTData() ---------------------------------------------------------


    protected int internalColor()
    {
        int numColorTypes = super.internalColor();
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if(colorFunction == numColorTypes + 1) // colour by Track Segment
        {
            colorBy(getSegments());
        }
        else if(colorFunction == numColorTypes + 2) // colour by isOutlier
        {
            for(int i=0; i<numDraw; i++)
            {
                int list = listdl[i];
                int iO = getIsOutlier(list);
                if(iO==0)
                    color[list]= AColorMap.GN;//green
                else if(iO==1)
                    color[list]= AColorMap.RD;//red
                else
                    color[list]= (byte) parameterStore.get(PARAMETER_GROUP, "Constant").getI();
            }
        }
        else if(colorFunction == numColorTypes + 3) // colour by driftSign
        {
            for(int i=0; i<numDraw; i++)
            {
                int list = listdl[i];
                int ds = getDriftSign(list);
                if(ds==-1)
                    color[list]= AColorMap.GN;//green
                else if(ds==1)
                    color[list]= AColorMap.RD;//red
                else
                    color[list]= (byte) parameterStore.get(PARAMETER_GROUP, "Constant").getI();
            }
        }
        else if(colorFunction == numColorTypes + 4) // colour by threshold
        {
            for(int i=0; i<numDraw; i++)
            {
                int list = listdl[i];
                if(threshold[list]==1)
                    color[list]= AColorMap.RD; // red is preferable then previous green
                else
                    color[list]= (byte) parameterStore.get(PARAMETER_GROUP, "Constant").getI();
            }
        }
        return numColorTypes + 1;

    } // internalColor() ----------------------------------------------------



    public int getLayer(int id)
    {
        try {
            return AIdHelper.trtLayerWheel(id);
        } catch (AAtlantisException e) {
            return 0;
        }
        //return(id & 0x01F00000) >> 20; before using IDHelper
    }



    public int getSub(int id)
    {
        try {
            return AIdHelper.trtBarrelEndcap(id);
        } catch (AAtlantisException e) {
            return 0;
        }
        //return(id & 0x06000000) >> 25; before using IDHelper
    }



    protected void applyCuts()
    {
        cutIndex();
        cut("CutsInDet", "Threshold", " Threshold", threshold);
        cutSimulatedTracks();
        cutReconstructedTracks();
        cutSegments();
        cutPhi(phi);
        cutEta();

        // cut noise
        if(noise != null && parameterStore.get("CutsInDet", "Noise").getStatus())
        {
            cutArray(noise, 0, "TRT Noise Cut");
        }

        // cut TimeOverThreshold
        // method takes care that the cut is actually turned on
        // or if the timeOverThreshold array is null (n/a in the event file)
        cut("CutsInDet", "TRTTimeOverThreshold",
            "TRTTimeOverThreshold", timeOverThreshold);
    }



    private void keepBarrel()
    {
        int num = 0;
        int cutSub = parameterStore.get("CutsInDet", "TRT").getI();
        for(int i = 0; i < numDraw; i++)
        {
            // sub values 1 and 2 - barrel
            if(sub[listdl[i]] == 1 || sub[listdl[i]] == 2)
            {
                if(cutSub == -1 || sub[listdl[i]] == cutSub)
                {
                    listdl[num++] = listdl[i];
                }
            }
        }
        numDraw = num;
    }



    private void keepEndcap()
    {
        int num = 0;
        for(int i = 0; i < numDraw; i++)
        {
            // sub values 0 and 3 - endcaps
            if(sub[listdl[i]] == 0 || sub[listdl[i]] == 3)
            {
                listdl[num++] = listdl[i];
            }
        }
        numDraw = num;
    }



    private void cutEta()
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");
        if(!par.getStatus())return;
        double etaCut = par.getD();
        double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
        double etaLowerCut = etaMid - etaCut;
        double etaUpperCut = etaMid + etaCut;
        int num = 0;
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double z1, z2, r1, r2;
            if(sub[list] == 0 || sub[list] == 3)
            {
                // endcap
                z1 = rhoz[list];
                z2 = rhoz[list];
                r1 = 64.;
                if(Math.abs(rhoz[list]) > 280.) r1 = 48.;
                r2 = 103.;
            }
            else
            {
                // barrel
                z1 = 0.35;
                if(rhoz[list] < 62) z1 = 40.;
                z2 = 74.2;
                if(sub[list] == 1)
                {
                    z1 *= -1.;
                    z2 *= -1.;
                }
                r1 = rhoz[list];
                r2 = rhoz[list];
            }
            double eta1 = AParameterUtilities.eta(z1, r1);
            double eta2 = AParameterUtilities.eta(z2, r2);
            double etaLower = Math.min(eta1, eta2);
            double etaUpper = Math.max(eta1, eta2);
            if(etaUpper > etaLowerCut && etaLower < etaUpperCut)
                listdl[num++] = list;
        }
        numDraw = num;
    }


    // in XY only TRT barrel data are displayed
    protected ACoord getYXUser()
    {
        makeDrawList();
        keepBarrel();
        double zoomScaling=0;
        //check if zoomed in enough to display circles/drift sign arrows
        Point2D.Double[] corners = ACanvas.getCanvas().getPaintingWindow().getUserCorners();
        double widthx=corners[1].x-corners[0].x;
        double widthy=corners[0].y-corners[2].y;
        //average width of window
        double width=Math.sqrt(widthx*widthx+widthy*widthy);
        //calculate scaling (100 is arbitrary constant)
        zoomScaling=100/width;
        //now return relevant user coords depending on the zoom
        //(0.25 is arbitrary constant increase to turn on drawing
        //    circles and arrows at a more zoomed in state)
        if(zoomScaling>0.25)
            return getYXUserCircle();
        else
            return getYXUserNormal();
    }


    /**
     * Will give coords for a line along the TRT drift
     */
    private ACoord getYXUserNormal()
    {
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double r = rhoz[list];
            double cosPhi = Math.cos(phi[list]);
            double sinPhi = Math.sin(phi[list]);
            double d = driftR[list];
            double x = r * cosPhi;
            double y = r * sinPhi;
            hv[0][0][i] = x + d * sinPhi;
            hv[1][0][i] = y - d * cosPhi;
            hv[0][1][i] = x - d * sinPhi;
            hv[1][1][i] = y + d * cosPhi;
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    }


    /**
     * Will give coords for the drift circle and arrow showing the drift
     */
    private ACoord getYXUserCircle()
    {
        int numPoints = 48;
        int[] index = new int[numDraw];
        double[][][] hv = new double[2][numDraw][];
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int ds = getDriftSign(list);
            //adjust ds if z<0
            if(sub[list] == 1)
            {
                ds *= -1;
            }
            if(ds==1 || ds==-1)
            {
                //has drift sign so has extra points for arrow
                hv[0][i] = new double[numPoints+3];
                hv[1][i] = new double[numPoints+3];
            }
            else
            {
                hv[0][i] = new double[numPoints];
                hv[1][i] = new double[numPoints];
                ds=0;
            }
            double r = rhoz[list];
            double cosPhi = Math.cos(phi[list]);
            double sinPhi = Math.sin(phi[list]);
            double d = driftR[list];
            double x = r * cosPhi;
            double y = r * sinPhi;
            int jstart=0;
            //calculate starting point of circle
            if(ds==1 || ds ==-1)
            {
                jstart = (int) (phi[list]/(Math.PI * 2 / (numPoints - 1)));
                d*=ds;
            }
            //store points of circle
            int count=0;
            for (int j = jstart; j < numPoints; j++)
            {
                hv[0][i][count] = x + d * Math.sin(Math.PI * 2 * j / (numPoints - 1));
                hv[1][i][count] = y - d * Math.cos(Math.PI * 2 * j / (numPoints - 1));
                count++;
            }
            for (int j = 0; j < jstart; j++)
            {
                hv[0][i][count] = x + d * Math.sin(Math.PI * 2 * (j+1) / (numPoints - 1));
                hv[1][i][count] = y - d * Math.cos(Math.PI * 2 * (j+1) / (numPoints - 1));
                count++;
            }
            index[i] = list;
            //if has drift show arrow
            if(ds==1 || ds==-1)
            {
                //first line
                hv[0][i][numPoints] = hv[0][i][2*(numPoints-1)/5];
                hv[1][i][numPoints] = hv[1][i][2*(numPoints-1)/5];
                //back to end point of circle
                hv[0][i][numPoints+1] = hv[0][i][numPoints-1];
                hv[1][i][numPoints+1] = hv[1][i][numPoints-1];
                //second line
                hv[0][i][numPoints+2] = hv[0][i][3*(numPoints-1)/5];
                hv[1][i][numPoints+2] = hv[1][i][3*(numPoints-1)/5];
              }
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES);
    }


    protected ACoord getRZUser()
    {
        // correction for primary vertex
        if(!parameterStore.get("RZ", "TRT").getStatus())return ACoord.NO_DATA;
        makeDrawList();
        // line size was found using findRZActiveArea(){
        // may need to be found again if geometry changes...
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            if(sub[list] == 0 || sub[list] == 3)
            {
                // endcap
                hv[0][0][i] = rhoz[list];
                hv[0][1][i] = rhoz[list];
                double phiDiff = Math.abs(phi[list] - phiMid);
                double r1 = 64.;
                if(Math.abs(rhoz[list]) > 280.) r1 = 48.;
                double r2 = 103.;
                if(phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                {
                    hv[1][0][i] = r1;
                    hv[1][1][i] = r2;
                }
                else
                {
                    hv[1][0][i] = -r1;
                    hv[1][1][i] = -r2;
                }
            }
            else
            {
                // barrel
                double z1 = 0.35;
                if(rhoz[list] < 62) z1 = 40.;
                double z2 = 74.2;
                if(sub[list] == 1)
                {
                    z1 *= -1.;
                    z2 *= -1.;
                }
                hv[0][0][i] = z1;
                hv[0][1][i] = z2;
                double phiDiff = Math.abs(phi[list] - phiMid);
                if(phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                {
                    hv[1][0][i] = rhoz[list];
                    hv[1][1][i] = rhoz[list];
                }
                else
                {
                    hv[1][0][i] = -rhoz[list];
                    hv[1][1][i] = -rhoz[list];
                }
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    }


    protected ACoord getFRUser()
    {
        makeDrawList();
        keepBarrel();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        double[] pVtx = event.getPrimaryVertex();
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double r = rhoz[list];
            double p = phi[list];
            double x = r * Math.cos(p);
            double y = r * Math.sin(p);
            double dx = x - pVtx[0];
            double dy = y - pVtx[1];
            r = Math.sqrt(dx * dx + dy * dy);
            p = Math.atan2(dy, dx);
            if(p < 0.) p += AMath.TWO_PI;
            double delPhi = driftR[list] / r;
            hv[0][0][i] = r;
            hv[1][0][i] = Math.toDegrees(p - delPhi);
            hv[0][1][i] = r;
            hv[1][1][i] = Math.toDegrees(p + delPhi);
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES).includePhiWrapAround("FR");
    }



    protected ACoord getFZUser()
    {
        makeDrawList();
        keepEndcap();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double z = rhoz[list];
            // average rho from findRZActiveArea()
            double averageRho = 81.;
            if(Math.abs(z) > 280.)
                averageRho = 70.;
            double p = phi[list];
            double delPhi = driftR[list] / averageRho;
            hv[0][0][i] = z;
            hv[1][0][i] = Math.toDegrees(p - delPhi);
            hv[0][1][i] = z;
            hv[1][1][i] = Math.toDegrees(p + delPhi);
            index[i] = list;
        }
        return new ACoord(hv, index, this,
                ACoord.LINES).includePhiWrapAround("FZ");
    }


    public Action[] getActions(Collection nodes)
    {
        if(nodes.size() != 1)
            return new Action[0];
        final DefaultMutableTreeNode node = (DefaultMutableTreeNode) (nodes.
                iterator().next());
        AList[] list = AListManager.getInstance().getChildren(node);
        if(list == null)return new Action[0];
        Set hits = new HashSet();
        for(int i = 0; i < list.length; ++i)
        {
            if(list[i].getSource() == this)
            {
                for(int j = 0; j < list[i].getItems().length; j++)
                {
                    hits.add(new Integer(list[i].getItems()[j]));
                }
            }
            else
            {
                return new Action[0];
            }
        }
        int numHits = hits.size();
        if(numHits < 2)
            return new Action[0];
        final int[] h = new int[numHits];
        Iterator i = hits.iterator();
        int n = 0;
        while(i.hasNext())
        {
            h[n++] = ((Integer) (i.next())).intValue();
        }
        Action[] action = new Action[1];
        action[0] = new AbstractAction("Find Eta")
        {
            public void actionPerformed(ActionEvent e)
            {
                final double MIN = 9999.;
                final double MAX = -9999.;
                double minRho = MIN;
                double maxRho = MAX;
                double minZ = MIN;
                double maxZ = MAX;
                for(int i = 0; i < h.length; ++i)
                {
                    if(sub[h[i]] == 1 || sub[h[i]] == 2)
                    {
                        minRho = Math.min(minRho, rhoz[h[i]]);
                        maxRho = Math.max(maxRho, rhoz[h[i]]);
                    }
                    else
                    {
                        minZ = Math.min(minZ, rhoz[h[i]]);
                        maxZ = Math.max(maxZ, rhoz[h[i]]);
                    }
                }
                AOutput.append("Eta results:\n  minRho = " +
                        String.format("%.3f",minRho) +
                        "  maxRho = " + String.format("%.3f",maxRho) + "\n" +
                        "  minZ = " + String.format("%.3f",minZ) +
                        "  maxZ = " + String.format("%.3f",maxZ) + "\n",
                        ALogInterface.NORMAL);
            }
        };
        return action;
    }


  @Override
    protected void finalizeConstruction() {
        super.finalizeConstruction();
        Vector keys = (Vector) event.getCollections().get("InDetTrack");
        if(keys != null)
        {
            Iterator keysIterator = keys.iterator();
            //loop over collections
            while(keysIterator.hasNext())
            {
                String trackCollec = (String) keysIterator.next();
                AInDetTrackData data = (AInDetTrackData) (event.get("InDetTrack" + trackCollec));
                int[][] hits = event.getAssociationManager().get("InDetTrack" + trackCollec, getName());
                int[][] drifts = event.getAssociationManager().get("InDetTrack" + trackCollec + "Drift", getName());
                int[][] outliers = event.getAssociationManager().get("InDetTrack" + trackCollec + "Outlier", getName());

                if(drifts!=null || outliers!=null)
                {
                    int[] driftSignData = new int[numData];
                    int[] isOutlierData = new int[numData];
                    //set to noData to distinguish from a 0 returned if the data is present
                    for(int i=0; i<this.numData; i++)
                    {
                        driftSignData[i]=NO_DATA;
                        isOutlierData[i]=NO_DATA;
                    }
                    //loop over alll hits in track collection
                    for(int i=0; i<data.getNumData(); i++)
                    {
                        for(int j=0; j<hits[i].length; j++)
                        {
                            //check if current hit matches a TRT index
                            int list=getIndexFromId(hits[i][j]);
                            if(list!=-1)
                            {
                                //found match so save value if not null
                                if(drifts!=null)
                                    driftSignData[list] = drifts[i][j];
                                if(outliers!=null)
                                    isOutlierData[list] = outliers[i][j];
                            }
                        }
                    }
                    //if has data in file then save array of values
                    if(drifts!=null)
                        driftSign.put(trackCollec, driftSignData);
                    if(outliers!=null)
                        isOutlier.put(trackCollec, isOutlierData);
                }
                //no data for this collection in file so save null to vector
                if(drifts==null)
                    driftSign.put(trackCollec, null);
                if(outliers==null)
                    isOutlier.put(trackCollec, null);
            }
        }
    }
}
