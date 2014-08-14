// NOTE: revert to previous version once support for Java 1.5 is dropped

package atlantis.gui;

import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.globals.AGlobals;
import java.awt.Desktop;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.lang.Thread.UncaughtExceptionHandler;
import java.net.URI;
import java.net.URLEncoder;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;

/**
 * This class handles any unhandled exception (program crash). It asks the user for
 * permission to submit a crash report by e-mail and pre-formats the message. It then
 * fires the default system mail client with this message to allow the user to add
 * additional commands and send it.
 *
 * @author Eric Jansen
 */
public class ACrashReporter implements UncaughtExceptionHandler {

    // Dialog asking for permission to report the crash
    private static final String title = "An uncaught exception occured";
    private static final String msg =
            "An uncaught exception occured!\n\n"
            + "Please help us prevent this in the future by reporting this incident.\n"
            + "If you click yes, Atlantis will try to open your mail client and create\n"
            + "a preformatted crash report for you. You can review the information\n"
            + "and add additional comments before it is sent.\n\n"
            + "Do you want to report this crash?";

    // Address that receives crash reports, chopped up in case spiders go through this code
    private static final String address = "atlas-atlantis-developers" + "@" + "cern" + "." + "ch";

    // Tag for easy filtering, prefix for the mail subject
    private static final String tag = "[crash-report] ";

    // System properties included in the report
    private static final String[] properties = {
        "java.version", "java.vendor", "java.vm.specification.version",
        "java.vm.specification.vendor", "java.vm.specification.name", "java.vm.version",
        "java.vm.vendor", "java.vm.name", "java.specification.version", "java.specification.vendor",
        "java.specification.name", "os.name", "os.arch", "os.version"};

    /** Flag indicating whether CrashReporter has already been shown */
    private static boolean shownCrashReporter = false;
    
    /**
     * Exception handler for uncaught exceptions in Atlantis. The only way to
     * "handle" such an exception is to tell the developers that they failed.
     * So that is what we will do.
     * @param t thread
     * @param e uncaught exception
     */
    public void uncaughtException(Thread t, Throwable e) {
    	// Ensure this only happens once
    	if (shownCrashReporter) return;
    	shownCrashReporter = true;
    	
        int confirm = JOptionPane.showConfirmDialog(null, msg, title,
                JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null);

        if (confirm == JOptionPane.YES_OPTION) {
            String report = "### Please explain briefly what you did when the problem occured (optional)\n\n";

            report += "\n### Exception information\n\n";
            report += getExceptionInformation(e);

            report += "\n### General information\n\n";
            report += getGeneralInformation();

            report += "\n### Event information\n\n";
            report += getEventInformation();

            // If possible, we open the mail client and create a new message
            if (Desktop.isDesktopSupported()) {
                Desktop desktop = Desktop.getDesktop();

                if (desktop.isSupported(Desktop.Action.MAIL)) {
                    String subject = getSubject(e);
                    try {
                        URI uri = new URI("mailto:" + address + "?SUBJECT="
                                + encode(subject) + "&BODY=" + encode(report));
                        desktop.mail(uri);
                    } catch (Throwable ex) {
                        // Something went wrong, save report to file instead
                        saveToFile(report);
                    }
                } else {
                    // No mail support, save report to file instead
                    saveToFile(report);
                }
            } else {
                // No desktop support, save report to file instead
                saveToFile(report);
            }
        }
    }

    /**
     * Provides mail encoding of a string (URLEncoding but with %20 for space)
     * @param s string to encode
     * @return encoded string
     */
    private static String encode(String s) {
        try {
            return URLEncoder.encode(s, "utf-8").replaceAll("\\+", "%20");
        } catch (Throwable ex) {
            return s;
        }
    }

    /**
     * This provides is the backup option if starting the mail client doesn't
     * work. The report will be saved to a local file and we ask the user to
     * send it.
     * @param report crash report contents
     */
    private void saveToFile(String report) {
        String directory = AGlobals.instance().getHomeDirectory();
        JFileChooser chooser = new JFileChooser(directory);
        chooser.setDialogTitle("Save crash report");
        chooser.setSelectedFile(new File("crash-report.txt"));

        // Remind people where this report should go at the top of the file
        report = "### Atlantis crash report, please send to " + address + "\n\n"
                + report;

        // Continue asking until the user chooses a valid file or cancels
        while (chooser.showSaveDialog(null) == JFileChooser.APPROVE_OPTION) {
            File file = chooser.getSelectedFile();
            try {
                OutputStream out = new FileOutputStream(file.getAbsolutePath());
                out.write(report.getBytes());
                out.close();

                JOptionPane.showMessageDialog(null, "The crash report has been saved successfully.\n"
                        + "Please send the file to " + address);
                return;
            } catch (Throwable ex) {
                JOptionPane.showMessageDialog(null, "Cannot write file: " + ex.toString());
            }
        }
    }

    /**
     * Takes the exception and finds the most helpful mail subject, in general
     * the exact location of the crash should be helpful when dealing with a 
     * lot of reports.
     * @param e uncaught exception
     * @return subject string
     */
    private static String getSubject(Throwable e) {
        StackTraceElement[] st = e.getStackTrace();
        if (st.length > 0) {
            return tag + st[0].toString();
        } else {
            return tag + e.toString();
        }
    }

    /**
     * Formats general information about the system, Java and Atlantis
     * @return information string
     */
    private static String getGeneralInformation() {
        StringBuilder s = new StringBuilder();
        s.append("Atlantis version = " + AGlobals.instance().getVersion() + "\n");

        for (String property : properties) {
            String value = System.getProperty(property);
            if (value == null) {
                value = "(not set)";
            }
            s.append(property + " = " + value + "\n");
        }

        return s.toString();
    }

    /**
     * Formats stack trace and exception information
     * @param e unhandled exception
     * @return information string
     */
    private static String getExceptionInformation(Throwable e) {
        StringBuilder s = new StringBuilder();
        String pad = "";

        s.append(e.toString() + "\n\n");

        StackTraceElement[] st = e.getStackTrace();
        for (StackTraceElement traceline : st) {
            s.append(pad + traceline + "\n");
            pad = "  ";
        }

        return s.toString();
    }

    /**
     * Formats event information, such as source, file, run/event and datatypes
     * @return information string
     */
    private static String getEventInformation() {
        StringBuilder s = new StringBuilder();
        AEventManager eventManager = AEventManager.instance();
        AEvent event = eventManager.getCurrentEvent();
        if (event == null) {
            return "(no current event)";
        }

        s.append("Source = " + event.getSourceName() + "\n");
        s.append("Run = " + event.getRunNumber() + "\n");
        s.append("Event = " + event.getEventNumber() + "\n");

        s.append("\nData types present:\n");
        String[][] info = event.getInfo();
        for (String[] item : info) {
            s.append("  " + item[0] + " (" + item[1] + ")\n");
        }

        return s.toString();
    }
}
