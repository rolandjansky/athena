package atlantis.data;

import atlantis.event.*;
import atlantis.graphics.colormap.AColorMap;
import atlantis.list.AListManager;
import atlantis.parameters.AEnumeratorParameter;
import atlantis.utils.AHashMap;
import java.util.Vector;
import atlantis.utils.ALogger;

public abstract class AHitData extends AData
{
    private static ALogger logger = ALogger.getLogger(AHitData.class);
    
    protected byte[] type;
    // sub should be removed at some point
    protected int[] sub;
    // linked list
    protected int[] ll;
    protected int[] llStart;
    protected int[] llNum;

    // for "new Truth" (2006-08-08) implementation of barcode management
    // (association to truth tracks)
    protected int[] barcode = null; // old style barcode
    protected int[] barcodes = null; // new style barcodes
    protected int[] numBarcodes = null; // new style numBarcodes

  @Override
    public final int getNumTypes(){
        return 2;
    }

    private static final int NOISE = 0;
    private static final int GOOD = 1;



    AHitData(AHashMap p, AEvent e)
    {
        super(p,e);
        
        sub = p.getUnknownIntArray("sub");
        type = new byte[numData];
        ll = new int[numData];
        llStart = new int[getNumTypes()];
        llNum = new int[getNumTypes()];

        AAssociation assoc = null;
        if (p.get("barcode") != null)
        {
            // old style truth (1-to-1 barcode association)
            barcode = p.getUnknownIntArray("barcode");

            assoc = new AAssociation(getFullName(), "STr", null, barcode, event);
            event.getAssociationManager().add(assoc);
        }
        else if(p.get("barcodes") != null && p.get("numBarcodes") != null)
        {
            // new style truth (1-to-n barcodes, numBarcodes association)
            barcodes = p.getUnknownIntArray("barcodes");
            numBarcodes = p.getUnknownIntArray("numBarcodes");

            // getFullName() should return hit datatype name + storegate key
            assoc = new AAssociation(getFullName(), "STr", numBarcodes, barcodes,event);
            event.getAssociationManager().add(assoc);
        }

    } // AHitData() ---------------------------------------------------------



    public String getHitInfo(int index)
    {
        StringBuilder r = new StringBuilder();
        if(this.barcode != null)
        {
            // old style truth association - 1 barcode associated with a hit
            r.append("\n barcode = " + barcode[index]);
        }
        else if(this.barcodes != null && this.numBarcodes != null)
        {
            // new style truth association - N barcodes associated with a hit

            int[][] barcodes = event.getAssociationManager().get(getFullName(), "STr");
            if(barcodes[index] != null)
            {
                r.append("\n barcode(s) = ");
                for(int i = 0; i < barcodes[index].length; i++)
                {
                    r.append(barcodes[index][i] + "  ");
                }
            }
            else
            {
                r.append("\n no STr associated");
            }
        }

        return r.toString();

    } // getHitInfo() -------------------------------------------------------



    /**
     * Calculates actual association via - SiCluster for the datatype
     * (SpacePoint (S3D) and TrigSpacePoint) to InDetTrack* (inner detector
     * reconstructed track collection)
     * @param to String
     * @param via String
     * @return int[][]
     */
    private int[][] calculateAssociation(String to, String via)
    {
        String infoMsg = "   association from: " + this.getName() + "  to: " +
                         to + "  via: " + via;
        
        logger.debug("AData.calculateAssociation()\n" + infoMsg);

        int[][] clusters = event.getAssociationManager().get(getFullName(), via);
        int[][] recon = event.getAssociationManager().get(via, to);
        AData source = event.get(via);
        if(clusters == null || recon == null || source == null)
        {
            return null;
        }

        int[][] ass = new int[numData][];

        // iterate over all data items of this datatype
        for(int i = 0; i < numData; ++i)
        {
            if(clusters[i] != null && clusters[i].length == 2)
            {
                // this is a SCT hit which has two clusters
                int index1 = source.getIndexFromId(clusters[i][0]);
                int index2 = source.getIndexFromId(clusters[i][1]);
                if(index1 != -1 && index2 != -1 &&
                   recon[index1] != null && recon[index2] != null)
                {
                    int[] temp = new int[recon[index1].length];
                    int matches = 0;
                    for(int k = 0; k < recon[index1].length; ++k)
                    {
                        for(int j = 0; j < recon[index2].length; ++j)
                        {
                            if(recon[index1][k] == recon[index2][j])
                            {
                                // IndexOutOfBound protection
                                if(matches < recon[index1].length)
                                {
                                    temp[matches] = recon[index1][k];
                                    matches++;
                                }
                                else
                                {
                                    logger.warn("Association problem:\n" + infoMsg);
                                }
                            }
                        }
                    }
                    if(matches == 0)
                    {
                        ass[i] = null;
                    }
                    else if(matches == temp.length)
                    {
                        ass[i] = temp;
                    }
                    else
                    {
                        int[] dest = new int[matches];
                        System.arraycopy(temp, 0, dest, 0, matches);
                        ass[i] = dest;
                    }
                }
            }
            else if(clusters[i] != null && clusters[i].length == 1)
            {
                // this is pixel hit which has one cluster only (second number was
                // -1 and was ignored in the datatypes (S3D, TrigS3D) constuctor

                // although this method calculates association via SiCluster
                // there are no SiClusters for Pixel and this is a Pixel hit
                // -> need to calculate the SpacePoint - Track association directly
                // based on Track:hits (which has got id of non-existing SiCluster
                // hits) and SpacePoint:cluster which also has Pixel cluster
                // references (more see email on 2007-09-20 association problems ...)
                // can't use above block as source (SiCluster) doesn't exists, there
                // are no SiCluster in Pixel, thus source.getIndexFromId(clusters[i][0])
                // can't work ...

                // AAssociationManager.get(via, to); would return recalculated
                // inverted association containing indices.

                // this returns the direct array saved in InDetTrackData() constructor
                // AAssociationManager.add(new AAssociation(assocKey, "SiCluster", numHits, hits));
                int[][] directRecon = event.getAssociationManager().getAssociation(to, via).getData();

                for(int x = 0; x < directRecon.length; x++)
                {
                    if(directRecon[x] != null)
                    {
                        for(int y = 0; y < directRecon[x].length; y++)
                        {
                            // if the SpacePoint cluster ID is the same as id in
                            // Track:hits, then it's an associated hit
                            if(clusters[i][0] == directRecon[x][y])
                            {
                                if(ass[i] == null)
                                {
                                    // current association, x is associated track index
                                    ass[i] = new int[] { x };
                                }
                                else
                                {
                                    // some tracks before were already associated
                                    // copy the array and add current track index
                                    int[] temp = new int[ass[i].length + 1];
                                    System.arraycopy(ass[i], 0, temp, 0, ass[i].length);
                                    temp[ass[i].length] = x; // current association
                                    ass[i] = new int[temp.length];
                                    System.arraycopy(temp, 0, ass[i], 0, temp.length);
                                }

                            }
                        } // for(int y = 0; y < directRecon[x].length; y++)
                    }
                } // loop over x (x is all track indices)
            } // else - pixel cluster found

        } // for(int i = 0; i < numData; ++i) - loop over all current datatype items

        return ass;

    } // calculateAssociation() ---------------------------------------------



    /*
     * This method uses clusters array (association with SiCluster) which
     * is accessed in calculateAssociation() and creates
     * TrigS3D - Track association and S3D - Track association (via SiCluster)
     */
    protected void calculateAssociationViaClusters()
    {
        Vector<String> keys = event.getCollections().get("InDetTrack");
        int[][] recon = null;
        int[][] recon2 = null;
        AAssociation assoc = null;
        String[] trackCollec = null;

        if(keys != null)
        {
            trackCollec = (String[]) keys.toArray(new String[keys.size()]);
            for(int j = 0; j < trackCollec.length; j++)
            {
                String trackCollecFull = "InDetTrack" + trackCollec[j];
                recon = event.getAssociationManager().get(getFullName(), trackCollecFull);
                recon2 = calculateAssociation(trackCollecFull, "SiCluster");
                if(recon != null && recon2 != null)
                {
                    for(int i = 0; i< numData; i++)
                    {
                        if(recon2[i] != null)
                        {
                            // using array reference here - association array is corrected
                            recon[i] = recon2[i];
                        }
                    }
                    assoc = event.getAssociationManager().getAssociation(getName(), trackCollecFull);
                    event.getAssociationManager().add(assoc.invert());
                }
            }
        }

    } // calculateAssociationViaClusters() ---------------------------------



    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if (colorFunction == 0)
        {
            colorByConstant();
        }
        else if (colorFunction == 1)
        {
            colorBy("STr");
        }
        else if (colorFunction == 2)
        {
            colorBy(getReconstructedTracks());
        }
        else if (colorFunction == 3)
        {
            colorBy(sub);
        }

        return 3;

    } // internalColor() ----------------------------------------------------



    protected String getSegments()
    {
        AEnumeratorParameter key = (AEnumeratorParameter) parameterStore.get("InDetSegment", "InDetSegmentCollections");

        return "InDetSegment" + key.getCurrentText();
    }



    public void constructDefaultDrawlist()
    {
        // make seperate lists of noise and good hits
        makeNoiseList(0, 0);

        if (!parameterStore.get("CutsATLAS", "ByList").getStatus())
        {
            // put noise and/or good hits in drawlist according to HitType
            // parameter
            // if both are drawn noise is drawn first
            makeDrawList(parameterStore.get("CutsInDet", "HitType").getI());

            // if some hits are on a list they must be drawn last
            int[][] temp = AListManager.getInstance().getColorMapping(this);
            int[] index = temp[0];
            int[] ctemp = temp[1];
            if (index.length > 0)
            {
                int[] c = new int[numData];
                final int NONE = -999;
                // color is not used at this time
                for (int i = 0; i < c.length; ++i)
                    c[i] = NONE;

                for (int i = 0; i < index.length; ++i)
                    c[index[i]] = ctemp[i];

                int[] newListdl = new int[numData];
                int num = 0;
                for (int t = 0; t < getNumTypes(); t++)
                {
                    for (int i = 0; i < numDraw; ++i)
                        if (t == type[listdl[i]] && c[listdl[i]] == NONE)
                            newListdl[num++] = listdl[i];
                    for (int i = 0; i < numDraw; ++i)
                        if (t == type[listdl[i]] && c[listdl[i]] != NONE && c[listdl[i]] != AColorMap.INVISIBLE)
                            newListdl[num++] = listdl[i];
                }
                listdl = newListdl;
                numDraw = num;
            }
        }
        else
        {
            boolean[] selected = AListManager.getInstance().getSelection(this);
            numDraw = 0;
            for (int t = 0; t < getNumTypes(); t++)
                for (int i = 0; i < numData; ++i)
                    if (t == type[i] && selected[i])
                        listdl[numDraw++] = i;
        }
    }


    /**
     * define noise and good hits type = 0 means noise
     * noise hits - hits associated neither with Track not STr
     * good hits - hits associated either with Track or with STr
     */
    protected void setType()
    {
        int[][] assocSTr = event.getAssociationManager().get(getFullName(), "STr");
        int[][] assocRTr = event.getAssociationManager().get(getName(), getReconstructedTracks());
                
        for (int i = 0; i < numData; i++)
        {
            if((assocSTr != null && assocSTr[i] != null) ||
               (assocRTr != null && assocRTr[i] != null))
            {
                type[i] = 1;
            }
            else
            {
                type[i] = 0;
            }
        }

    } // setType() ----------------------------------------------------------



    protected void makeNoiseList(int a, int b)
    {
        setType();

        for (int i = 0; i < getNumTypes(); i++)
        {
            llStart[i] = -1;
            llNum[i] = 0;
        }

        for (int i = numData - 1; i >= 0; i--)
        {
            ll[i] = llStart[type[i]];
            llStart[type[i]] = i;
            llNum[type[i]]++;
        }
    }

    private void makeDrawList(int hitType)
    {
        int start, end;

        numDraw = 0;
        if (hitType == 0)
        {
            start = end = NOISE;
        }
        else if (hitType == 1)
        {
            start = end = GOOD;
        }
        else
        {
            start = NOISE;
            end = GOOD;
        }

        for (int i = start; i <= end; i++)
        {
            int n = llStart[i];

            for (int j = 0; j < llNum[i]; j++)
            {
                listdl[numDraw++] = n;
                n = ll[n];
            }
        }
    }



    public int[] getType(int[] dl)
    {
        int[] temp = new int[dl.length];

        for (int i = 0; i < temp.length; i++)
            temp[i] = type[dl[i]];
        return temp;
    }



    /**
     * cut hits if connected/unconnected to simulated tracks
     */
    protected void cutSimulatedTracks()
    {
        int cutOption = parameterStore.get("CutsInDet", "HitsBySTr").getI();
        cutByAssociationTo("STr", getFullName(), cutOption);
    }
    
    /**
     * cut hits if connected/unconnected to reconstructed tracks
     */
    protected void cutReconstructedTracks()
    {
        int cutOption = parameterStore.get("CutsInDet", "HitsByRTr").getI();
        cutByAssociationTo(getReconstructedTracks(), getName(), cutOption);
    }
    
    /**
     * cut hits if connected/unconnected to reconstructed segments
     */
    protected void cutSegments()
    {
        int cutOption = parameterStore.get("CutsInDet", "HitsBySegment").getI();
        cutByAssociationTo(getSegments(), getName(), cutOption);
    }
}
