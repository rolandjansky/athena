package atlantis.gui;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.Timer;
import javax.swing.JButton;

/**
 * This class is not used at the moment.
 * Was used to control animation.
 */
public class AControlButton extends JButton {

  public static final int STANDARD_ACTION=0;
  public static final int ACCELERATED_ACTION=1;

  private ActionListener controlListener;
  private ActionEvent standardAction, acceleratedAction;
  private long ticks;
  private Timer timer;
  private int initialDelay;
  private int tickLength;
  private int accelerationLimit;
  private boolean ignoreClick=false;

  public AControlButton(String text, int initialDelay, int tickLength, int accelerationLimit) {
    super(text);
    this.initialDelay=initialDelay;
    this.tickLength=tickLength;
    this.accelerationLimit=accelerationLimit;
    initialize();
  }

  private void initialize() {
    standardAction=new ActionEvent(this, 0, "", STANDARD_ACTION);
    acceleratedAction=new ActionEvent(this, 0, "", ACCELERATED_ACTION);

    timer=new Timer(tickLength, new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        ticks++;
        fireControlActionPerformed();
        ignoreClick=true;
      }
    });
    timer.setInitialDelay(initialDelay);

    addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        ticks=0;
        timer.restart();
      }

      public void mouseReleased(MouseEvent e) {
        timer.stop();
      }

      public void mouseClicked(MouseEvent e) {
        if(ignoreClick)
          ignoreClick=false;
        else
          fireControlActionPerformed();
      }
    });

  }

  private void fireControlActionPerformed() {
    if(controlListener!=null) {
      if(ticks>accelerationLimit)
        controlListener.actionPerformed(acceleratedAction);
      else
        controlListener.actionPerformed(standardAction);
    }
  }

  public void addControlActionListener(ActionListener controlListener) {
    if(this.controlListener!=null)
      throw new Error("AControlButton supports only 1 listener");
    this.controlListener=controlListener;
  }

}
