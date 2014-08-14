package atlantis.data;

import atlantis.event.*;
import atlantis.utils.AHashMap;

/**
 *
 * Muon Region Of Interest
 *
 * @author Zdenek Maxa, Qiang Lu, Juergen Thomas
 */
public class AMuonROIData extends AROIData
{
    AMuonROIData(AHashMap p, AEvent e)
    {
        super(p,e);
    } // AMuonROIData() ------------------------------------------------------

    public String getParameterGroup()
    {
        return "MuonROI";
    } // getParameterGroup() ------------------------------------------------

    public String getName()
    {
        return "MuonROI";
    } // getName() ----------------------------------------------------------

    public String getNameScreenName()
    {
        return "MuonROI";
    } // getNameScreenName() ------------------------------------------------

} // class AMuonROIData ======================================================
