package atlantis.data;

import atlantis.event.*;
import atlantis.utils.AHashMap;

/**
 *
 * Em Tau Region Of Interest
 *
 * @author Zdenek Maxa, Qiang Lu
 */
public class AEmTauROIData extends AROIData
{
    AEmTauROIData(AHashMap p, AEvent e)
    {
        super(p,e);
    } // AEmTauROIData() ----------------------------------------------------

    public String getParameterGroup()
    {
        return "EmTauROI";
    }

    public String getName()
    {
        return "EmTauROI";
    }

    public String getNameScreenName()
    {
        return "EmTauROI";
    }

} // class AEmTauROIData ====================================================
