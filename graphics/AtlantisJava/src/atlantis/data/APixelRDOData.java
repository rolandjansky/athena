package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;


/*
 * PixelRDO - Pixel Raw Data object (SpacePoint Raw Data Objects)
 * The class is derived from AS3DData class (SpacePoint)
 * Parameters expected in the event file for the PixelRDO datatype
 *     id x y z etaModule phiModule
 */
public class APixelRDOData extends AS3DData
{
    private int[] etaModule = null;
    private int[] phiModule = null;


    public String getParameterGroup()
    {
        return "PixelRDO";

    } // getParameterGroup() ------------------------------------------------



    public String getName()
    {
        return "PixelRDO";

    } // getName() ----------------------------------------------------------



    public String getNameScreenName()
    {
        return "PixelRDO";

    } // getNameScreenName() ------------------------------------------------



    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+"\n"+
        AMath.RHO+" = "+String.format("%.3f",rho[index])+"\n "+
        "z = "+String.format("%.3f",z[index])+" cm\n"+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        
        String temp = getNameScreenName() + " (id: " + id[index] + " index: " + index + ")" +
                      "\n " + AMath.RHO + " = " + String.format("%.3f",rho[index]) + " cm" +
                      "\n " + AMath.PHI + " = " +
                      String.format("%.3f",Math.toDegrees(phi[index])) +
                      AMath.DEGREES + "  (" + String.format("%.3f",phi[index]) + " rad)" +
                      "\n z = " + String.format("%.3f",z[index]) + " cm";

        temp += "\n " + AMath.ETA + " module = ";
        temp += (etaModule != null) ? Integer.toString(etaModule[index]) : "n/a";
        temp += "\n " + AMath.PHI + " module = ";
        temp += (phiModule != null) ? Integer.toString(phiModule[index]) : "n/a";

        return temp;

    } // getHitInfo() -------------------------------------------------------



    APixelRDOData(AHashMap p, AEvent e)
    {
        super(p,e);
        id = p.getIntArray("id");
        etaModule = (p.get("etaModule") != null) ? p.getIntArray("etaModule") : null;
        phiModule = (p.get("phiModule") != null) ? p.getIntArray("phiModule") : null;

    } // APixelRDOData() ----------------------------------------------------



    protected int internalColor()
    {
        int numColorTypes = super.internalColor();
        // int colorFunction=APar.get(PARAMETER_GROUP, "ColorFunction").getI();
        // color by error will go here
        // if(colorFunction==numColorTypes+1)
        // colorByError() - to implement

        return numColorTypes + 1;

    } // internalColor() ----------------------------------------------------



    protected void applyCuts()
    {
        cutIndex();
        cutPhi(phi);
        cutEta(rho, z);
        if(etaModule != null)
        {
            cut("CutsInDet", "EtaModule", " EtaModule", etaModule);
        }
        if(phiModule != null)
        {
            cut("CutsInDet", "PhiModule", " PhiModule", phiModule);
        }

        cutSubdetector();
    } // applyCuts() --------------------------------------------------------

    private void cutSubdetector()
    {
        AParameter subPar = parameterStore.get("CutsInDet", "SCT");
        if(subPar.getI() != -1)
            cutArray(sub, subPar.getI(), "Barrel/Endcap");
    }
    
    protected ACoord getFRUser()
    {
        makeDrawList();
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index = new int[numDraw];

        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            h[i] = rho[list];
            v[i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[list], phi[list], z[list]));
            index[i] = list;
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("FR");

    } // getFRUser() --------------------------------------------------------



    protected ACoord getFZUser()
    {
        makeDrawList();
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            h[i] = z[list];
            v[i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[list], phi[list], z[list]));
            index[i] = list;
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("FZ");

    } // getFZUser() --------------------------------------------------------

} // class APixelRDOData ====================================================
