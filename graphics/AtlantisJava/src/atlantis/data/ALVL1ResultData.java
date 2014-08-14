package atlantis.data;

import atlantis.event.AData;
import atlantis.event.*;
import atlantis.utils.AHashMap;

public class ALVL1ResultData extends AData
{
    private String[] ctpItemList;
    private String[] itemListL2;
    private String[] itemListEF;
    private int[] ctpWord0;
    private int[] ctpWord1;
    private int[] ctpWord2;
    private float[] energyEtMiss;
    //private float[] energyEx;
    //private float[] energyEy;
    private float[] energySumEt;
    private int[] passedL1;
    private int[] passedL2;
    private int[] passedEF;
    private int[] passedTrigger;
    private String[] prescaleListEF;
    private String[] prescaleListL1;
    private String[] prescaleListL2;
    
    ALVL1ResultData(AHashMap p, AEvent e)
    {
        super(p,e);

        ctpItemList = p.getStringArray("ctpItemList");
        itemListL2 = p.getStringArray("itemListL2");
        itemListEF = p.getStringArray("itemListEF");
        ctpWord0 = p.getUnknownIntArray("ctpWord0");
        ctpWord1 = p.getUnknownIntArray("ctpWord1");
        ctpWord2 = p.getUnknownIntArray("ctpWord2");
        energyEtMiss = p.getUnknownFloatArray("energyEtMiss");
        //energyEx = p.getUnknownFloatArray("energyEx");
        //energyEy = p.getUnknownFloatArray("energyEy");
        energySumEt = p.getUnknownFloatArray("energySumEt");
        passedL1 = p.getUnknownIntArray("passedL1");
        passedL2 = p.getUnknownIntArray("passedL2");
        passedEF = p.getUnknownIntArray("passedEF");
        passedTrigger = p.getUnknownIntArray("passedTrigger");
        //check for backward compatibility
        prescaleListEF = (p.get("prescaleListEF") != null) ? p.getStringArray("prescaleListEF"): null;
        prescaleListL1 = (p.get("prescaleListL1") != null) ? p.getStringArray("prescaleListL1"): null;
        prescaleListL2 = (p.get("prescaleListL2") != null) ? p.getStringArray("prescaleListL2"): null;
    }

    protected void applyCuts()
    {}
    
    public String getHitInfo(int index)
    {
        return "LVL1Result";
    }

    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if(colorFunction == 0)
            colorByConstant();

        return 3;
    }

    public String getParameterGroup()
    {
        return "LVL1Result";
    }

    public String getNameScreenName()
    {
        return "LVL1Result";
    }

    public String getCtpItemList(int index)
    {
        return ctpItemList[index];
    }
    
    /**
     * Returns the individual parts of the list seperated by -
     * By using cleanup removes L1 from the item
     */
    public String[] getCtpItemListSplit(int index, boolean cleanUp)
    {
        if(ctpItemList!=null)
        {
            if(cleanUp)
                return splitItems(ctpItemList[index], "L1");
            else
                return splitItems(ctpItemList[index]);
        }
        else
            return null;
    }
    
    public String getitemListL2(int index)
    {
        return itemListL2[index];
    }

    /**
     * Returns the individual parts of the list seperated by -
     * By using cleanup removes L2 from the item
     */
    public String[] getitemListL2Split(int index, boolean cleanUp)
    {
        if(itemListL2!=null)
        {
            if(cleanUp)
                return splitItems(itemListL2[index], "L2");
            else
                return splitItems(itemListL2[index]);
        }
        else
            return null;
    }
    
    public String getitemListEF(int index)
    {
        return itemListEF[index];
    }

    /**
     * Returns the individual parts of the list seperated by -
     * By using cleanup removes EF from the item
     */
    public String[] getitemListEFSplit(int index, boolean cleanUp)
    {
        if(itemListEF!=null)
        {
            if(cleanUp)
                return splitItems(itemListEF[index], "EF");
            else
                return splitItems(itemListEF[index]);
        }
        else
            return null;
    }
    
    public int getCtpWord0(int index)
    {
        return ctpWord0[index];        
    }
    
    public String getBinaryCtpWord0(int index)
    {
        return Integer.toBinaryString(ctpWord0[index]);        
    }
    
    public int getCtpWord1(int index)
    {
        return ctpWord1[index];
    }
    
    public int getCtpWord2(int index)
    {
        return ctpWord2[index];
    }
   
    /**
     * This method is needed for backward compatibility
     * New xml files store this in ATriggerInfoData
     */
    public float getEnergyEtMiss(int index)
    {
        return energyEtMiss[index];
    }
    
    /*public float getEnergyEx(int index)
    {
        return energyEx[index];
    }
    public float getEnergyEy(int index)
    {
        return energyEy[index];
    }*/
   
    /**
     * This method is needed for backward compatibility
     * New xml files store this in ATriggerInfoData
     */
    public float getEnergySumEt(int index)
    {
        return energySumEt[index];
    }
    
    public int getPassedL1(int index)
    {
        return passedL1[index];
    }
    
    public int getPassedL2(int index)
    {
        return passedL2[index];
    }
    
    public int getPassedEF(int index)
    {
        return passedEF[index];
    }
    
    public int getPassedTrigger(int index)
    {
        return passedTrigger[index];
    }
    
    public String getPrescaleListEF(int index)
    {
        return prescaleListEF[index];
    }
    
    /**
     * Returns the individual parts of the list seperated by -
     */
    public String[] getPrescaleListEFSplit(int index)
    {
        if(prescaleListEF!=null)
            return splitItems(prescaleListEF[index]);
        else
            return null;
    }
    
    public String getPrescaleListL1(int index)
    {
        return prescaleListL1[index];
    }
    
    /**
     * Returns the individual parts of the list seperated by -
     */
    public String[] getPrescaleListL1Split(int index)
    {
        if(prescaleListL1!=null)
            return splitItems(prescaleListL1[index]);
        else
            return null;
    }
    
    public String getPrescaleListL2(int index)
    {
        return prescaleListL2[index];
    }
    
    /**
     * Returns the individual parts of the list seperated by -
     */
    public String[] getPrescaleListL2Split(int index)
    {
        if(prescaleListL2!=null)
            return splitItems(prescaleListL2[index]);
        else
            return null;
    }
    
    /**
     * Function that seperates an item list and carries out cleanup
     */
    private String[] splitItems(String origItems, String level)
    {
        //string array to hold individual items
        String[] newItems=splitItems(origItems);
        //remove letters from start of item and duplicated names
        char[] levelChar = level.toCharArray();
        for(int i=0; i<newItems.length; i++)
        {
            char[] newItemsChar = newItems[i].toCharArray();
            if(newItemsChar[0]==levelChar[0] && newItemsChar[1]==levelChar[1])
            {
                if(newItemsChar[2]=='_')
                    newItems[i]=newItems[i].substring(3);
                else
                    newItems[i]=newItems[i].substring(2);
                //get rid of duplicated parts of name
                char[] newItemsChar2 = newItems[i].toCharArray();
                for(int j=0; j<newItemsChar2.length-1; j++)
                {
                    if(newItemsChar2[j]==levelChar[0] && newItemsChar2[j+1]==levelChar[1])
                    {
                        newItems[i]=newItems[i].substring(0, j);
                        break;
                    }
                }
            }
        }
        return newItems;
    }   
}
