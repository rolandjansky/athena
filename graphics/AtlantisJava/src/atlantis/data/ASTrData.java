package atlantis.data;

import atlantis.event.*;
import atlantis.utils.AHashMap;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;

public class ASTrData extends ATrackData
{
    private static ALogger logger = ALogger.getLogger(ASTrData.class);
    
    public static final int UNDERLYING_EVENT = 0;
    public static final int NORMAL = 1;

    protected int[] code;
    protected int[] type;
    protected int[] particleType;
    protected int[] originVertex;

    public String getParameterGroup()
    {
        return "STr";
    }

    public String getName()
    {
        return "STr";
    }

    public String getNameScreenName()
    {
        return "SimChargedTrack";
    }

    ASTrData(AHashMap p, AEvent e) throws AAtlantisException
    {
        super(p,e);
        code = p.getIntArray("code");
        originVertex = p.getIntArray("originVertex");
        type = p.getIntArray("type");
        particleType = new int[numData];
        for(int i = 0; i < numData; i++)
            particleType[i] = APDGTable.getParticleType(code[i]);
    }


    public static AHashMap createSTr(AHashMap p)
    {
        float[] pt = p.getFloatArray("pt");
        float[] phi = p.getFloatArray("phi");
        float[] eta = p.getFloatArray("eta");
        float[] rhoVertex = p.getFloatArray("rhoVertex");
        float[] phiVertex = p.getFloatArray("phiVertex");
        float[] zVertex = p.getFloatArray("zVertex");
        int[] id = p.getIntArray("id");
        int[] code = p.getIntArray("code");
        //rhoEndVertex not available in older xml files - check that it exists first.
        float[] rhoEndVertex = (p.get("rhoEndVertex") != null) ? p.getFloatArray("rhoEndVertex") : null;
        //float[] phiEndVertex = (p.get("phiEndVertex") != null) ? p.getFloatArray("phiEndVertex") : null;
        float[] zEndVertex = (p.get("zEndVertex") != null) ? p.getFloatArray("zEndVertex") : null;
        
        int numCharged = 0;
        for(int i = 0; i < code.length; ++i)
        {
            int charge = code[i];

            try
            {
                if(Math.abs(code[i]) >= 10 || Math.abs(code[i]) == 1)
                    // code is being used to store particle code
                    charge = APDGTable.getCharge(code[i]);
                if(charge == 1 || charge == -1)
                    numCharged++;
            }
            catch(APDGTable.ParticleNotFoundError pnf){
              logger.debug(pnf.getMessage());
            }
        }
 
        //Assign the vertices to the particle tracks
        int[] simulatedVertex = ASVxData.assignVertexNumbers(phiVertex, rhoVertex, zVertex);

        //Now build subsets of the charged tracks only
        int[] idC = new int[numCharged];
        int[] typeC = new int[numCharged];
        float[] rhoVertexC = new float[numCharged];
        float[] rhoEndVertexC = new float[numCharged];
        float[] phiVertexC = new float[numCharged];
        float[] zVertexC = new float[numCharged];
        float[] ptC = new float[numCharged];
        float[] phiC = new float[numCharged];
        float[] etaC = new float[numCharged];
        int[] chargeC = new int[numCharged];
        int[] codeC = new int[numCharged];
        int[] simulatedVertexC = new int[numCharged];

        numCharged = 0;
        for(int i = 0; i < code.length; ++i)
        {
            int charge = code[i];
            try
            {
                if(Math.abs(code[i]) >= 10 || Math.abs(code[i]) == 1)
                    // code is being used to store particle code
                    charge = APDGTable.getCharge(code[i]);
                if(charge == 1 || charge == -1)
                {
                    if(id[i] > 0)
                    {
                        idC[numCharged] = id[i];
                        typeC[numCharged] = NORMAL;
                    }
                    else
                    {
                        // negative id is used to flag tracks belong to a second
                        // track bank which correspond to the underlyingEvent
                        // set id to zero as it is not a kine number
                        idC[numCharged] = 0;
                        typeC[numCharged] = UNDERLYING_EVENT;
                    }
                    //copy all the information
                    rhoVertexC[numCharged] = (float)(Math.sqrt(rhoVertex[i]*rhoVertex[i]-zVertex[i]*zVertex[i]));
                    //if there is no end vertex, fill with 0.0 as in XML file
                    rhoEndVertexC[numCharged] = (rhoEndVertex != null) ? (float)(Math.sqrt(rhoEndVertex[i]*rhoEndVertex[i]-zEndVertex[i]*zEndVertex[i])) : 0 ;
                    phiVertexC[numCharged] = phiVertex[i];
                    zVertexC[numCharged] = zVertex[i];
                    ptC[numCharged] = pt[i];
                    //if (pt[i]>5) System.out.println(zVertex[i]);
                    phiC[numCharged] = phi[i];
                    etaC[numCharged] = eta[i];
                    codeC[numCharged] = code[i];
                    chargeC[numCharged] = charge;
                    simulatedVertexC[numCharged] = simulatedVertex[i];
                    numCharged++;
                }
            }
            catch(APDGTable.ParticleNotFoundError pnf)
            {
                logger.debug(pnf.getMessage());
            }
        }

        //Build a new hash map with the information for the 
        //charged tracks only
        AHashMap newP = new AHashMap(12);
        newP.put("numData", new Integer(numCharged));
        newP.put("id", idC);
        newP.put("type", typeC);
        newP.put("rhoVertex", rhoVertexC);
        newP.put("rhoEndVertex", rhoEndVertexC);
        newP.put("phiVertex", phiVertexC);
        newP.put("zVertex", zVertexC);
        newP.put("pt", ptC);
        newP.put("phi", phiC);
        newP.put("eta", etaC);
        newP.put("code", codeC);
        newP.put("charge", chargeC);
        newP.put("originVertex", simulatedVertexC);
        //return the new hasp map
        return newP;
    }


    /**
     * Get the vertex for which sum|Pt| is maximum
     * @return the index of that vertex
     */
    public int getMaxSumPtVertex()
    {
       /***
        * Triple loop can be avoided, e.g using hashmap,
        * to tired to fix all this - S.B.
        */
      
       //get the highest vertex number
       int maxVertex = 0;

       //by looping over all tracks and looking at the vertex number
       for(int i = 0; i < numData; i++)
         if(type[i] == NORMAL)
           if(originVertex[i] > maxVertex)
             maxVertex = originVertex[i];
       
       //Now make an array to hold the sumPT for all vertices
       double[] sumPt = new double[maxVertex + 1];

       //Now loop again to add up all the pT
       for(int i = 0; i < numData; i++)
         if(type[i] == NORMAL && Math.abs(h[i].d0()) < 2.0)
           sumPt[originVertex[i]] += Math.abs(h[i].pT());
       
       //Yippee - lets loop once more to find the highest pT one
       int VtxIndex = 0;
       for(int i = 0; i < sumPt.length; i++)
         if(sumPt[i] > sumPt[VtxIndex])
           VtxIndex = i;

        return VtxIndex;
    }



    protected void applyCuts()
    {
        super.applyCuts();

        // STr are always drawn as helices, check for tracks with rhoVertex
        // beyond the diameter of InDet and don't draw those
        super.cutRhoVertexAfterInDetRadius();

        cut("CutsInDet", "STr", " STr id", id);
        cut("CutsInDet", "STrCode", " STr code", code);
        cut("CutsInDet", "STrType", " STr Type", particleType);
        cut("CutsInDet", "SVx", " Vertex", originVertex);
    }

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
                colorBy("ParticleColors", particleType);
                break;
            case 4:
                colorBy(originVertex);
                break;
        }

        return 4;
    }

    public String getHitInfo(int index)
    {
        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (barcode/id: " + id[index] + " index: " + index + ")");
        msg.append("\n Type = "+APDGTable.getName(code[index]));
        msg.append(" (type code "); msg.append(code[index]);
        msg.append(")"); msg.append(h[index].toString());

        return msg.toString();
    }

}
