package atlantis.interactions;

import java.awt.*;
import java.awt.geom.*;

public class AScaleCopyInteraction extends AInteraction {

  public AScaleCopyInteraction() {
    super(1, AUTOMATIC_REPAINT, SCREEN_COORDINATES, false);
    hr[0]=new Rectangle2D.Double(0, 0, 0, 0);
  }

  public void paint(Graphics2D g) {}

  public void connect(AInteractionsManager manager) {
    super.connect(manager);
    window.setScaleCopyStatus(true);
  }

  public void disconnect() {
    window.setScaleCopyStatus(false);
  }

}
