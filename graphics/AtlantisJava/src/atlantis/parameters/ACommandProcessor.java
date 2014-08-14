package atlantis.parameters;

import java.util.Vector;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.event.AEventManager;
import atlantis.event.AFilter;
import atlantis.data.AS3DData;
import atlantis.data.ATrackData;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.projection.AProjection;
import atlantis.utils.ALogger;

/**
 * (almost?) All actions on the GUI generate a command (which is basically a
 * text string) which is passed to the command processor which performs the
 * actions associated with that command. This should allow a simple
 * implementation of scripting and logging
 *
 * NOTE: This design has been dramatically neglected in the past and should
 *       probably be revised (Sebastian Boeser, 06.10.09)
 */

public class ACommandProcessor
{
    private static ALogger logger = ALogger.getLogger(ACommandProcessor.class);
    private static AEventManager eventManager = AEventManager.instance();
    
    private static String legoLayoutName ="";
    private static String[] legoWindowNames = {"current", "W", "1", "2", "3", "4"};
    private final static String[] legoStatusParameterNames = {"DrawPlot", "MainLegend", "L1EtLegend", "L1Items", "L2Items", "EFItems"};
    private final static String[] legoIntParameterNames = {"Mode", "nphicells", "netacells"};
    private final static String[] legoCaloParameterNames = {"LAr", "TILE", "FCAL", "MBTS"};
    private static boolean[][] legoStatusParameter = new boolean[6][6];
    private static int[][] legoIntParameter = new int[6][3];
    private static boolean[][] legoCaloParameter = new boolean[2][4];
    
    private static APar parameterStore = APar.instance();

    /**
     * While this is called receive, it is actually just a small wrapper to
     * processes a command and at the same time log it
     * @param command the command to process
     */
    public synchronized static void receive(String command)
    {
      //In debug mode show the command
      logger.debug("ACommandProcessor.receive(): " + command);
      //In trace mode show also who called this
      logger.trace("Calling hierarchy",new Exception());

      //Now process the command
      boolean status = process(command);

      //And log the result
      logger.debug( ((status)?"  success: ":"   bad command:") + command);

    }



    private synchronized static boolean process(String c)
    {

        if (processGlobal(c))
        {
            return true;
        }

        AProjection p = ACanvas.getCanvas().getCurrentWindow().getProjection();
        // projection dependent command could be processed here
        if (p.processLocalCommand(c))
        {
            return true;
        }

        if (parameterStore.processParameterChangeCommand(c))
        {
            return true;
        }

        return false;
    }

    // used for checking all command are implemented
    private synchronized static boolean found(int mode, String name, String command)
    {
        if (mode != 0)
        {
            logger.warn("Command not found: " + command);
            return false;
        }
        if (name.equals(command))
            return true;
        return false;
    }

    private synchronized static boolean foundWindow(int mode, String name, String command)
    {
        if (mode != 0)
        {
            String[] windowNames = ACanvas.getCanvas().getKnownWindowNames();

            for (int i = 0; i < windowNames.length; ++i)
                logger.debug(command + windowNames[i]);
            return false;
        }

        if (name.substring(0, name.length() - 1).equals(command))
            if (ACanvas.getCanvas().isValidWindowName(name.substring(name.length() - 1)))
                if (ACanvas.getCanvas().getWindow(name.substring(name.length() - 1)) != null)
                    return true;
        return false;
    }

    private synchronized static boolean processGlobal(String c)
    {
        return processGlobal(c, 0);
    }

    private static void getGlobalCommands()
    {
        processGlobal("GARY", 1);
    }

    private static boolean processZoomTrack(String name, int mode)
    {
        ATrackData tr = null;

        // check if STr (Simulated Track) Zoom Next Track was pressed
        if(found(mode, name, "ZOOMNEXTSTR"))
        {
            tr = eventManager.getCurrentEvent().getSTrData();
        }
        // check if Track (InDetTrack) Zoom Next Track was pressed
        else if(found(mode, name, "ZOOMNEXTTRACK"))
        {
            AEnumeratorParameter listBox = null;
            listBox = (AEnumeratorParameter) parameterStore.get("InDetTrack", "InDetTrackCollections");
            String col = listBox.getCurrentText();
            if("All".equals(col))
            {
                String msg = "Choose a particular Track collection, " +
                             "can't zoom next track when 'All' is selected.\n";
                AOutput.alwaysAppend(msg, ALogInterface.WARNING);
                return false;
            }
            else
            {
                tr = eventManager.getCurrentEvent().getTrackData("InDetTrack", col);
            }
        }

        if(tr == null)
        {
            // STr or Track of requested collection doesn't exits in this event
            return false;
        }

        // check if the datatype where Zoom Next Track was pressed is actually
        // turned on and thus visible
        if(! parameterStore.get("Data", tr.getName()).getStatus())
        {
            AOutput.alwaysAppend("Can't zoom next track, " + tr.getNameScreenName() +
                           " is turned off.\n", ALogInterface.WARNING);
            return false;
        }

        AParameter nextTrPar = parameterStore.get(tr.getName(), "NextTrack");
        nextTrPar.setStatus(true);
        nextTrPar.setI(nextTrPar.getI() + 1);
        tr.zoomAroundTracks();
        ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();

        AOutput.alwaysAppend(tr.getNameScreenName() + " -> Next Track cut active now.\n",
                       ALogInterface.WARNING);

        return true;

    } // processZoomTrack()
    // ---------------------------------------------------

    private synchronized static boolean processGlobal(String name, int mode)
    {
        // is this a group in the parameter file ?
        Vector<String> groups = parameterStore.getUIGroupNames();
        for (int i = 0; i < groups.size(); ++i)
        {
            String groupName = groups.elementAt(i);
            if (found(mode, name, groupName + "."))
            {
                ACanvas.getCanvas().getCurrentWindow().setGroup(groupName);
                return true;
            }
        }

        if(found(mode, name, "SETORIGINALPRIMARYVERTEX"))
        {
            eventManager.getCurrentEvent().setPrimaryVertex();
            ACanvas.getCanvas().repaintAllFromScratch();
        }


        if (found(mode, name, "LEGOTRIGGER"))
        {
            processLegoTrigger();
            return true;
        }

        if (found(mode, name, "RETURNLEGOTRIGGER"))
        {
            if(legoLayoutName.equals(""))
            {
                AOutput.append("No trigger debugging to return from\n", ALogInterface.NORMAL);
                return true;
            }
            
            returnLegoTrigger();
            return true;
        }

        if (found(mode, name, "PREVSECTOR"))
        {
            AParameter phi = parameterStore.get("YZ", "Phi");
            phi.setD(((Math.round(phi.getD()/22.5) + 15) % 16) * 22.5);
            ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
            return true;
        }

        if (found(mode, name, "NEXTSECTOR"))
        {
            AParameter phi = parameterStore.get("YZ", "Phi");
            phi.setD(((Math.round(phi.getD()/22.5) + 1) % 16) * 22.5);
            ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
            return true;
        }

        if (found(mode, name, "ZOOMNEXTSTR") || found(mode, name, "ZOOMNEXTTRACK"))
        {
            return processZoomTrack(name, mode);
        }

        if (found(mode, name, "PC"))
        {
            getGlobalCommands();
            return true;
        }
        if (found(mode, name, "DO"))
        {
            ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
            return true;
        }

        // turning all datatypes on/off command: - see code which was
        // commented out in AtlantisJava-09-05-71 and before

        // Toggle: PT,D0,Z0,NU off:NG,NB,NS,NR,SH
        if (found(mode, name, "CC"))
        {
            if (parameterStore.get("CutsInDet", "z0-zVtx").getStatus())
            {
                parameterStore.get("CutsInDet", "z0-zVtx").setStatus(false);
                parameterStore.get("CutsInDet", "d0").setStatus(false);
                parameterStore.get("CutsInDet", "Pt").setStatus(false);
                parameterStore.get("CutsInDet", "NumS3D").setStatus(false);
            }
            else
            {
                parameterStore.get("CutsInDet", "z0-zVtx").setStatus(true);
                parameterStore.get("CutsInDet", "d0").setStatus(true);
                parameterStore.get("CutsInDet", "Pt").setStatus(true);
                parameterStore.get("CutsInDet", "NumS3D").setStatus(true);
            }
            parameterStore.get("CutsInDet", "RTrIndex").setStatus(false);
            parameterStore.get("CutsInDet", "STr").setStatus(false);
            parameterStore.get("CutsInDet", "Group").setStatus(false);
            parameterStore.get("CutsATLAS", "Index").setStatus(false);
            ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
            return true;
        }
        if (found(mode, name, "FH"))
        {
          //Try to get SpacePoints
          AS3DData spacePoints = (AS3DData) eventManager.getCurrentEvent().get("S3D");
          //Apply Hans Drevermanns hit filter
          if (spacePoints != null){
            AFilter.filter(spacePoints);
            ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
          }
          return true;
        }
        if (foundWindow(mode, name, "W"))
        {
            ACanvas.getCanvas().setCurrentWindow(name.substring(1, 2));
            return true;
        }
        if (foundWindow(mode, name, "C"))
        {
            return true;
        }
        if (foundWindow(mode, name, "Z"))
        {
            return true;
        }
        return false;
    } // processGlobal() ------------------------------------------------------

    private synchronized static void processLegoTrigger()
    {
        String currentWindowName = ACanvas.getCanvas().getCurrentWindowName();
        String currentLayoutName = ACanvas.getCanvas().getCurrentLayout().getName();

        //save status of windows on first time button pressed
        if(legoLayoutName.equals(""))
        {
            //output message to the user
            AOutput.append("\nWindows changed to:\n",ALogInterface.TITLE);
            AOutput.append("Window 1: LegoPlot with Calo View\n", ALogInterface.NORMAL);
            AOutput.append("Window 2: LegoPlot with Trigger Tower View\n", ALogInterface.NORMAL);
            AOutput.append("Window 3: LegoPlot with Jet Element View\n", ALogInterface.NORMAL);
            AOutput.append("Window 4: LegoPlot Legend (may not be displayed if window too small)\n", ALogInterface.NORMAL);
            //save current window name and layout to then parameters to restore later
            legoWindowNames[0]=currentWindowName;
            legoLayoutName=currentLayoutName;
            for(int i=0; i<legoWindowNames.length; i++)
            {
                parameterStore.selectWindowParameters(legoWindowNames[i]);
                if (ACanvas.getCanvas().getWindow(legoWindowNames[i]) != null)
                {
                    //save lego status parameters
                    for(int j=0; j<legoStatusParameterNames.length; j++){
                        legoStatusParameter[i][j]=parameterStore.get("LegoPlot", legoStatusParameterNames[j]).getStatus();
                    }
                    //save lego int parameters
                    for(int j=0; j<legoIntParameterNames.length; j++){
                        legoIntParameter[i][j]=parameterStore.get("LegoPlot", legoIntParameterNames[j]).getI();
                    }
                    //save data parameters for windows 1 and 4
                    if(legoWindowNames[i].equals("1")){
                        for(int j=0; j<legoCaloParameterNames.length; j++){
                            legoCaloParameter[0][j]=parameterStore.get("Data", legoCaloParameterNames[j]).getStatus();
                        }
                    }else if(legoWindowNames[i].equals("4")){
                        for(int j=0; j<legoCaloParameterNames.length; j++){
                            legoCaloParameter[1][j]=parameterStore.get("Data", legoCaloParameterNames[j]).getStatus();
                        }
                    }
                }
                parameterStore.restoreWindowParameters();
            }
        }
        //copy to W to make sure can switch layout to 4 squares
        ACanvas.getCanvas().copyWindowSettings(currentWindowName, "W");
        if(!currentLayoutName.equals("FOUR SQUARES"))
            ACanvas.getCanvas().setCurrentLayout("FOUR SQUARES");
        //for cell sizing take defaults values and then roughly scaled for different views
        int nphicells = 128;
        int netacells = 200;
        //now copy to each window and set the display correctly
        for (int i = 0; i < 4; i++)
        {
            String wName = Integer.toString(i + 1);
            AWindow w = ACanvas.getCanvas().getWindow(wName);
            parameterStore.selectWindowParameters(wName);
            if (w != null)
            {
                ACanvas.getCanvas().copyWindowSettings("W", wName);
                //first 3 windows show data items (calo, trigger towers, jet elements)
                if(i<3)
                {
                    parameterStore.get("LegoPlot", "Mode").setI(i);
                    parameterStore.get("LegoPlot", "DrawPlot").setStatus(true);
                    parameterStore.get("LegoPlot", "MainLegend").setStatus(false);
                    parameterStore.get("LegoPlot", "L1EtLegend").setStatus(false);
                    parameterStore.get("LegoPlot", "L1Items").setStatus(false);
                    parameterStore.get("LegoPlot", "L2Items").setStatus(false);
                    parameterStore.get("LegoPlot", "EFItems").setStatus(false);
                    parameterStore.get("LegoPlot", "nphicells").setI(nphicells/((int) Math.pow(2.0, i)));
                    parameterStore.get("LegoPlot", "netacells").setI(netacells/((int) Math.pow(2.0, i)));
                }
                else
                {
                    //last window shows legend
                    parameterStore.get("LegoPlot", "DrawPlot").setStatus(false);
                    parameterStore.get("LegoPlot", "MainLegend").setStatus(true);
                    parameterStore.get("LegoPlot", "L1EtLegend").setStatus(true);
                    parameterStore.get("LegoPlot", "L1Items").setStatus(true);
                    parameterStore.get("LegoPlot", "L2Items").setStatus(true);
                    parameterStore.get("LegoPlot", "EFItems").setStatus(true);
                }
                //if first or last window it needs the data items turned on
                if(i==0 || i==3)
                {
                    for(int j=0; j<legoCaloParameterNames.length; j++){
                        parameterStore.get("Data", legoCaloParameterNames[j]).setStatus(true);
                    }
                }
            }
            parameterStore.restoreWindowParameters();
            ACanvas.getCanvas().moveToFrontWindow(w.getName());
            w.repaintFromScratch();

        }
    }

    private synchronized static void returnLegoTrigger()
    {
        //now will reset the parameters explictly set by trigger debug mode
        //others may be changed as on doing trigger debug others might get changed
        //when the current window is copied to W but these have been changed by the
        //user so not so important to reset
        
        //loop backwards so can reset each window before reverting to previous layout
        for(int i=legoWindowNames.length-1; i>=0; i--)
        {
            //reset the lego status parameters 
            parameterStore.selectWindowParameters(legoWindowNames[i]);
            for(int j=0; j<legoStatusParameterNames.length; j++){
                parameterStore.get("LegoPlot", legoStatusParameterNames[j]).setStatus(legoStatusParameter[i][j]);
            }
            //reset the lego int parameters
            for(int j=0; j<legoIntParameterNames.length; j++){
                parameterStore.get("LegoPlot", legoIntParameterNames[j]).setI(legoIntParameter[i][j]);
            }
            //reset the data ites for windows 1 and 4
            if(legoWindowNames[i].equals("1")){
                for(int j=0; j<legoCaloParameterNames.length; j++){
                    parameterStore.get("Data", legoCaloParameterNames[j]).setStatus(legoCaloParameter[0][j]);
                }
            }else if(legoWindowNames[i].equals("4")){
                for(int j=0; j<legoCaloParameterNames.length; j++){
                    parameterStore.get("Data", legoCaloParameterNames[j]).setStatus(legoCaloParameter[1][j]);
                }
            }
            parameterStore.restoreWindowParameters();
            //finished altering window so repaint
            ACanvas.getCanvas().getWindow(legoWindowNames[i]).repaintFromScratch();
            //have just finished W window so now can reset the layout 
            if(i==1){
                ACanvas.getCanvas().setCurrentLayout(legoLayoutName);
                legoLayoutName="";
            }
        }
        //everything reset now make the original window where trigger debug
        //was started the current window
        ACanvas.getCanvas().setCurrentWindow(legoWindowNames[0]);
    }

}
