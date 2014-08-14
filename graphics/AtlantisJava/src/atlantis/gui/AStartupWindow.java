package atlantis.gui;

import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;
import java.awt.*;
import javax.swing.*;


/**
 * The startup window appears when the program starts up and displays the
 * progress of the operation. No need for this to be a singleton at all.
 */
public class AStartupWindow {

  //add a logger
  private final ALogger logger = ALogger.getLogger(AStartupWindow.class);

  //the frame showing the startup image and progress
  private final JFrame frame = (AGlobals.isAtlantisHeadless()) ? null : new JFrame();

  //how often we will update the progress bar
  private final int progressUpdateDiff;

  //GUI items that will be updated
  private JLabel text = null;
  private JProgressBar progress = null;

  public AStartupWindow(int numUpdates) {
    
    //Store how often we will update the progress bar
    progressUpdateDiff=(int)100./numUpdates;

    //Don't do anything in headless mode
    if (AGlobals.isAtlantisHeadless()) return;

    frame.getContentPane().setLayout(new BorderLayout());

    String fileName = AGlobals.instance().getHomeDirectory() + "img" +
    		System.getProperty("file.separator") + "atlas.jpg";
    ImageIcon icon = AUtilities.getFileAsImageIcon(fileName);

    frame.getContentPane().add(new JLabel(icon), BorderLayout.CENTER);
    text = new JLabel("Welcome to Atlantis", JLabel.CENTER);
    JPanel p = new JPanel();

    p.setLayout(new GridLayout(2, 1));
    p.add(text);
    progress = new JProgressBar(0, 100);
    progress.setStringPainted(true);
    p.add(progress);
    frame.getContentPane().add(p, BorderLayout.SOUTH);
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    frame.setLocation(screenSize.width / 4, screenSize.height / 3);
    frame.setUndecorated(true);
    AIcon.setIconImage(frame);
    frame.setTitle("Atlantis starting up...");
    frame.pack();
    frame.setVisible(true);
  }

  /**
   * Update the frame and log to the logger
   * @param newText the new text for the startup window
   */
  public void updateText(String newText) {
    //Also send this to the logger
    logger.info(newText);

    //Don't do graphics update in headless mode
    if (AGlobals.isAtlantisHeadless()) return;
    
    //Update text and progress bar
    text.setText(newText); 
    progress.setValue(progress.getValue()+progressUpdateDiff);
  }

  /**
   * When dispsoing this object, also dispose of the frame
   */
  public void dispose(){
    if (frame!=null) frame.dispose();
  }
  
}
