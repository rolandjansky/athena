package atlantis.data;

import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.utils.AHashMap;

public class ATriggerInfoData extends AData
{
    private float[] energyEtMiss;
    //private float[] energyEx;
    //private float[] energyEy;
    private float[] energySumEt;
    private String[] trigInfoEF;
    private String[] trigInfoExtL1ID;
    private String[] trigInfoL1;
    private String[] trigInfoL2;
    private String[] trigInfoLvl1Type;
    private String[] trigInfoStatus;
    private String[] trigInfoStreamTag;
        
    ATriggerInfoData(AHashMap p, AEvent e)
    {
        super(p,e);
        energyEtMiss = p.getUnknownFloatArray("energyEtMiss");
        //energyEx = p.getUnknownFloatArray("energyEx");
        //energyEy = p.getUnknownFloatArray("energyEy");	
        energySumEt = p.getUnknownFloatArray("energySumEt");
        trigInfoEF = p.getStringArray("trigInfoEF");
        trigInfoExtL1ID = p.getStringArray("trigInfoExtL1ID");
        trigInfoL1 = p.getStringArray("trigInfoL1");
        trigInfoL2 = p.getStringArray("trigInfoL2");
        trigInfoLvl1Type = p.getStringArray("trigInfoLvl1Type");
        trigInfoStatus = p.getStringArray("trigInfoStatus");
        trigInfoStreamTag = p.getStringArray("trigInfoStreamTag");
    }

    protected void applyCuts()
    {}
    
    public String getHitInfo(int index)
    {
        return "TriggerInfo";
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
        return "TriggerInfo";
    }

    public String getNameScreenName()
    {
        return "TriggerInfo";
    }
    
    /**
     * For backward compatibility if this is null or -1
     * also check the value returned from ALVL1ResultData
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
     * For backward compatibility if this is null or -1
     * also check the value returned from ALVL1ResultData
     */
    public float getEnergySumEt(int index)
    {
        return energySumEt[index];
    }
    
    public String getTrigInfoEF(int index)
    {
        return trigInfoEF[index];
    }
    
    /**
     * Returns the individual parts of the list seperated by -
     */
    public String[] getTrigInfoEFSplit(int index)
    {
        if(trigInfoEF!=null)
            return splitItems(trigInfoEF[index]);
        else
            return null;
    }
    
    /**
     * Returns the hex values of the
     * individual parts of the list seperated by -
     */
    public String[] getTrigInfoEFSplitHex(int index)
    {
        return hexValues(splitItems(trigInfoEF[index]));
    }
    
    public String getTrigInfoExtL1ID(int index)
    {
        return trigInfoExtL1ID[index];
    }

    public String getTrigInfoL1(int index)
    {
        return trigInfoL1[index];
    }
    
    /**
     * Returns the individual parts of the list seperated by -
     */
    public String[] getTrigInfoL1Split(int index)
    {
        if(trigInfoL1!=null)
            return splitItems(trigInfoL1[index]);
        else
            return null;
    }
    
    /**
     * Returns the hex values of the
     * individual parts of the list seperated by -
     */
    public String[] getTrigInfoL1SplitHex(int index)
    {
        return hexValues(splitItems(trigInfoL1[index]));
    }

    public String getTrigInfoL2(int index)
    {
        return trigInfoL2[index];
    }
    
    /**
     * Returns the individual parts of the list seperated by -
     */
    public String[] getTrigInfoL2Split(int index)
    {
        if(trigInfoL2!=null)
            return splitItems(trigInfoL2[index]);
        else
            return null;
    }
    
    /**
     * Returns the hex values of the
     * individual parts of the list seperated by -
     */
    public String[] getTrigInfoL2SplitHex(int index)
    {
        return hexValues(splitItems(trigInfoL2[index]));
    }
    
    public String getTrigInfoLvl1Type(int index)
    {
        return trigInfoLvl1Type[index];
    }
    
    public String getTrigInfoStatus(int index)
    {
        return trigInfoStatus[index];
    }
    

    public String getTrigInfoStreamTag(int index)
    {
        return trigInfoStreamTag[index];
    }
    
    /**
     * Returns the individual parts of the list seperated by -
     */
    public String[] getTrigInfoStreamTagSplit(int index)
    {
        if(trigInfoStreamTag!=null)
            return splitItems(trigInfoStreamTag[index]);
        else
            return null;
    }

            
    /**
     * Returns the hex values of each element in the string
     * string[] items represents a long[]
     */
    private String[] hexValues(String[] items)
    {
        for(int i=0; i<items.length; i++)
        {
            long longvalue= Long.valueOf(items[i]).longValue();
            items[i] = "0x"+Long.toString(longvalue, 16).toUpperCase();
        }
        return items;
    }
}
