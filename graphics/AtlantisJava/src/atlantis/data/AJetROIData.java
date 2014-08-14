package atlantis.data;

import atlantis.event.*;
import atlantis.utils.AHashMap;

/**
 *
 * Jet Region Of Interest
 *
 * @author Zdenek Maxa, Qiang Lu
 */
public class AJetROIData extends AROIData
{

    AJetROIData(AHashMap p, AEvent e)
    {
        super(p,e);
    } // AJetROIData() ------------------------------------------------------


    public String getParameterGroup()
    {
        return "JetROI";
    } // getParameterGroup() ------------------------------------------------


    public String getName()
    {
        return "JetROI";
    } // getName() ----------------------------------------------------------


    public String getNameScreenName()
    {
        return "JetROI";
    } // getNameScreenName() ------------------------------------------------

} // class AJetROIData ======================================================
