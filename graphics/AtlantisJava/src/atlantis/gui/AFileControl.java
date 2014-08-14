package atlantis.gui;

import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JMenu;
import javax.swing.JOptionPane;
import javax.swing.JSeparator;

import atlantis.config.ADefaultValues;
import atlantis.data.AG4StepData;
import atlantis.event.AEventManager;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;
import atlantis.globals.AGlobals;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.APar;
import atlantis.utils.ALogger;
import javax.swing.JFileChooser;


/**
 * The File menu in the GUI.
 */
public class AFileControl extends JMenu
{
    private static ALogger logger = ALogger.getLogger(AFileControl.class);
    private static AEventManager eventManager = AEventManager.instance();
    private static final AGlobals globals = AGlobals.instance();

    public AFileControl()
    {
        super("File");

        add("Read Event Locally").addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e)
            {
              readEventLocally(); 
            
            }
        }); // add("Read Event Locally")

        add("Read Event From URL (live)").addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                readEventFromURL();
            }
        }); // add("Read Event From URL")

        add(new JSeparator());

        add("Loop over events").addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                AEventLoopDialog.getInstance().showDialog();
            }
        });

        add(new JSeparator());

        add("Save Image of Canvas").addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
               //Create a new dialog
              ASaveCanvasDialog dialog = new ASaveCanvasDialog();
              //And show it
              dialog.showDialog();
            }
        });

        add(new JSeparator());

        add("Event Properties").addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                AEventPropertiesDialog.getInstance().setVisible(true);
            }
        }); // add("Event Properties")

        add(new JSeparator());

        add("Read Geometry").addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                new AReadGeometryChooser();
            }
        }); // add("Read Geometry")

        add("Read G4Steps").addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                AG4StepData.ReadG4Steps();
            }
        }); // add ("Read G4Steps")
        
        int userLevel = APar.instance().getUserLevel();
        if(userLevel>=3) // TODO: Use name for "debug" instead of magic number
        {
            add("Memory information").addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    System.gc();
                    long total = Runtime.getRuntime().totalMemory();
                    long free = Runtime.getRuntime().freeMemory();
                    long max = Runtime.getRuntime().maxMemory();
                    int fac = 1024 * 1024; // Mega factor
                    String newLine = System.getProperty("line.separator");
                    AOutput.append("\nMemory information:"+newLine, ALogInterface.NORMAL);
                    AOutput.append(" total: " + total + " B  " + total / fac + " MB"+newLine, ALogInterface.NORMAL);
                    AOutput.append(" free : " + free + " B  " + free / fac + " MB"+newLine, ALogInterface.NORMAL);
                    AOutput.append(" max  : " + max + " B  " + max / fac + " MB"+newLine, ALogInterface.NORMAL);
                }
            });
            
        } // if(logger.isDebugEnabled())

        add(new JSeparator());

        add("Exit").addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                new AClosingConfirmationDialog(globals.getGuiFrame()).exitApp();
            }
        });

    } // AFileControl() -----------------------------------------------------

    
    /**
     * Open a file dialog and read the selected event or directory
     */
    private static void readEventLocally(){
      Frame gui = globals.getGuiFrame();
      //get the last location from the default values  
      String lastLoc = ADefaultValues.get("LastEventFilesSourceDir");
      //Create a file chooser dialog
      AReadFileChooser fileDialog = new AReadFileChooser(lastLoc);
      //Check if the user pressed ok in the dialog
      if ( fileDialog.showOpenDialog(gui) == JFileChooser.APPROVE_OPTION) {
        //Get the selected file path, prepend file://
        String fullFilePath = "file://" + fileDialog.getSelectedFile().getPath();
        
        try {
          //Now set this as a new event source
          eventManager.setEventSource(fullFilePath);
          eventManager.nextEvent();
        } catch (NoMoreEventsException nme) {
          String msg = "No events found from this source!";
          JOptionPane.showMessageDialog(gui, msg, "No more events", JOptionPane.ERROR_MESSAGE);
          logger.warn(nme.getCauseMessages());
        } catch (InvalidEventSourceException ies ){
          String msg = "Not a valid event source";
          JOptionPane.showMessageDialog(gui, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
          logger.warn(ies.getCauseMessages());
        } catch (ReadEventException ree ){
          String msg = "Can not read event(s)";
          JOptionPane.showMessageDialog(gui, msg, "Read error", JOptionPane.ERROR_MESSAGE);
          logger.warn(ree.getCauseMessages());
        }
      } // okay pressed - otherwise do nothing
    }
    
    
    private static void readEventFromURL()
    {
        // URL may end with .zip - archive of event files, .xml - single
        // XML event file (in this case Read Next, Previous works on the web
        // directory starting from this single file) or URL only of a web
        // directory (string input considered to end with whatever else) -
        // in this case Atlantis gets the first event in the directory,
        // Read Next, Previous work as expected
        // this URL should always start with http://
    	
        Frame gui = globals.getGuiFrame();
        String urlString = JOptionPane.showInputDialog(gui,
                           "Enter URL:", AGlobals.instance().getLivePoint1EventsURL());
                
        //Make sure this is a proper string - otherwise abort
        if(urlString == null || urlString.trim().length()==0) return;
        
        // event manager further distinguishes the particular event
        // reader whether URL ends with .zip or .xml
        try {
          //Now set this as a new event source
          eventManager.setEventSource(urlString);
          eventManager.nextEvent();
        } catch (NoMoreEventsException nme) {
          String msg = "No events found from this source!";
          JOptionPane.showMessageDialog(gui, msg, "No more events", JOptionPane.ERROR_MESSAGE);
          logger.warn(nme.getCauseMessages());
        } catch (InvalidEventSourceException ies ){
          String msg = "Not a valid event source";
          JOptionPane.showMessageDialog(gui, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
          logger.warn(ies.getCauseMessages());
        } catch (ReadEventException ree ){
          String msg = "Can not read event(s)";
          JOptionPane.showMessageDialog(gui, msg, "Read error", JOptionPane.ERROR_MESSAGE);
          logger.warn(ree.getCauseMessages());
        }
    } // readEventFromURL() -------------------------------------------------
   
} // class AFileControl =====================================================
