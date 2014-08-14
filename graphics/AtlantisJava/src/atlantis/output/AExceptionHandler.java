package atlantis.output;

import javax.swing.JOptionPane;

import atlantis.globals.AGlobals;

/**
 * Handles an exception thrown in the AWT event-dispatch thread. If the system
 * property "sun.awt.exception.handler" is defined then when an exception occurs
 * in the AWT event-dispatch thread, the thread wii do the following: 1) Load
 * the class named by the value of that property, using the current thread's
 * context class loader, 2) Instantiate that class using its zero-argument
 * constructor, 3) Find the resulting handler object's "public void handle"
 * method, which should take a single argument of type "Throwable", and 4)
 * Invoke the handler's "handle" method, passing it the exception as an
 * argument.
 *
 * Note: The magic "sun.awt.exception.handler" property will be
 * removed in a future release.
 *
 * Comment: No wonder - centrally handling all exceptions is not a good idea
 * in the first place (S.B.)
 */

public class AExceptionHandler {
	private static final AGlobals globals = AGlobals.instance();
	
    public void handle(Throwable exception)
    {
        processException("AWT Event Dispatch Thread Exception:", exception);
    }

    public static void processException(String message, Throwable exception)
    {
        exception.printStackTrace();
        showMessageDialog(message);
    }

    public static void processException(String type, String message)
    {

        if (type.equals("fatal")){
            showMessageDialog(type + ": " + message + "\n");
            System.exit(1);
        }
        AOutput.append(type + ": " + message + "\n", ALogInterface.BAD_COMMAND);
    }

    private static void showMessageDialog(String message)
    {
      //Do not show messages in headless mode
      if(AGlobals.isAtlantisHeadless()) return;

      //Do not show messages in demo mode
      if (AGlobals.instance().isDemoMode()) return;

      //Else show a message box with the exception
      JOptionPane.showMessageDialog(globals.getGuiFrame(), message, "Atlantis", JOptionPane.ERROR_MESSAGE);
    }

}
