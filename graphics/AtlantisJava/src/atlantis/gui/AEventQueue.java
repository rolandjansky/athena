package atlantis.gui;

import java.awt.EventQueue;
import java.awt.AWTEvent;
import java.awt.Frame;
import java.awt.event.KeyEvent;
import atlantis.parameters.APar;
import atlantis.canvas.ACanvas;
import atlantis.data.AG4StepData;
import atlantis.event.AEventManager;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;
import atlantis.globals.AGlobals;
import atlantis.interactions.AModifier;
import javax.swing.JOptionPane;
import atlantis.utils.ALogger;

/**
 * Because we use Mouse Actions and Keyboard Modifiers (letters) we need a way of
 * getting all the keyboard events independent of the focussed compenent. This is
 * done by catching them directly from the Event Queue. So, we just replace the
 * default Java Queue by this one.
 */
public class AEventQueue extends EventQueue {

    /**
     * Contains the code of the key (is some key is pressed)
     * or KeyEvent.VK_UNDEFINED if no key is holded down.
     */
    private static int keyboardState;
    private static AModifier defInt = new AModifier(KeyEvent.VK_UNDEFINED, false, "");

    private KeyEvent keyEvent;

    private final static ALogger logger = ALogger.getLogger(AEventQueue.class);
    private static AEventManager eventManager = AEventManager.instance();

    private static boolean isRightMouseButton;
    
    protected static APar parameterStore = APar.instance();
    
    private static final AGlobals globals = AGlobals.instance();

    public static int getKeyboardState() {
        int ret = 0;

        if (keyboardState == KeyEvent.VK_UNDEFINED) {
            ret = defInt.getKeyCode();
        } else {
            ret = keyboardState;
        }

        // Bit of a hack because fastzoom is handled differently from other
        // interactions
        if (ret == KeyEvent.VK_F) {
            parameterStore.get("Projection", "SkipData").setStatus(true);
        } else {
            parameterStore.get("Projection", "SkipData").setStatus(false);
        }

        return ret;
    }

    public static AModifier getDefault() {
        return defInt;
    }

    public static void setDefault(AModifier mod) {
        defInt = mod;

    }

    public static int getKeyboardDefault() {
        return defInt.getKeyCode();
    }

    public static boolean getIntRightMouseButton() {
        if (keyboardState == KeyEvent.VK_UNDEFINED) {
            // Non-default action selected, use the right click option from it
            return defInt.getRightClick() || isRightMouseButton;
        } else {
            return isRightMouseButton;
        }
    }

    // FIXME: isRightMouseButton sometimes has spurious true value: see Trac #488
    protected void dispatchEvent(AWTEvent event) {
    	Frame gui = globals.getGuiFrame();
        super.dispatchEvent(event);

        if(event instanceof KeyEvent) {
            keyEvent=(KeyEvent)event;
// this is the right mouse click on single mouse button mac's
            switch(keyEvent.getID()) {
                case KeyEvent.KEY_PRESSED:
                    if(keyEvent.getKeyCode()==17) {
                        isRightMouseButton=true;
                        return;
                    }
                    keyboardState=keyEvent.getKeyCode();
                    //atlantis.utils.AOutput.append(" key pressed :" + keyboardState + ":\n", atlantis.utils.ALogPane.NORMAL_BOLD);

                    if (keyEvent.getKeyCode()==33){//page up
                        System.out.println("Go to previous event");
                        try {
                            eventManager.previousEvent();
                        } catch (NoMoreEventsException nme) {
                            String msg = "Already at first event from this source!";
                            JOptionPane.showMessageDialog(gui, msg, "No more events", JOptionPane.ERROR_MESSAGE);
                            logger.warn(nme.getCauseMessages());
                        } catch (InvalidEventSourceException ies) {
                            String msg = "Not a valid event source - please select one!";
                            JOptionPane.showMessageDialog(gui, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
                            logger.warn(ies.getCauseMessages());
                        } catch (ReadEventException ree) {
                            String msg = "Can not read event(s)!";
                            JOptionPane.showMessageDialog(gui, msg, "Read error", JOptionPane.ERROR_MESSAGE);
                            logger.warn(ree.getCauseMessages());
                        }
                    }
                    if (keyEvent.getKeyCode()==34) {//page down
                        System.out.println("Go to next event");
                        try {
                            eventManager.nextEvent();
                        } catch (NoMoreEventsException nme) {
                            String msg = "Already at last event from this source!";
                            JOptionPane.showMessageDialog(gui, msg, "No more events", JOptionPane.ERROR_MESSAGE);
                            logger.warn(nme.getCauseMessages());
                        } catch (InvalidEventSourceException ies) {
                            String msg = "Not a valid event source - please select one!";
                            JOptionPane.showMessageDialog(gui, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
                            logger.warn(ies.getCauseMessages());
                        } catch (ReadEventException ree) {
                            String msg = "Can not read event(s)!";
                            JOptionPane.showMessageDialog(gui, msg, "Read error", JOptionPane.ERROR_MESSAGE);
                            logger.warn(ree.getCauseMessages());
                        }
                    }
                    
                    if (keyEvent.getKeyCode()==47) {// "/"
                        AG4StepData.ReadG4Steps();
                    }//key "/"

                    if (keyEvent.getKeyCode() == KeyEvent.VK_F) {
                        parameterStore.get("Projection", "SkipData").setStatus(true);
                    }
                    break;

                case KeyEvent.KEY_RELEASED:
                    if (keyEvent.getKeyCode() == 17) {
                        isRightMouseButton = false;
                        return;
                    }
                    keyboardState = KeyEvent.VK_UNDEFINED;
                    if (keyEvent.getKeyCode() == KeyEvent.VK_F) {
                        parameterStore.get("Projection", "SkipData").setStatus(false);
                        ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
                    }
                    break;
            }
        }
    }

}
