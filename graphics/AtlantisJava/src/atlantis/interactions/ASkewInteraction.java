package atlantis.interactions;

import atlantis.canvas.AWindow;
import atlantis.parameters.APar;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjection;
import atlantis.projection.AProjectionRZ;
import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * The interaction that performs a skew on the picture.
 */
public class ASkewInteraction extends AInteraction
  implements ASleepMouseDragListener, ActionListener {

  private int d=20;
  private Point2D.Double p0;
  private JMenuItem[] popupItems;
  private final static String NO_SKEW="Set No Skew";

  public ASkewInteraction() {
    super(1, WINDOW_TOTAL_REPAINT, WORLD_COORDINATES, false);

    popupItems = new JMenuItem[] {
        new JMenuItem(NO_SKEW)
    };
    
    popupItems[0].addActionListener(this);
    
    // The center of the ellipse is in (0, 0) in User Space
    hr[0]=new Ellipse2D.Double(-d/2, -d/2, d, d);
  }

  public int init(Point2D.Double p, int key) {
    p0=p;
    return 0;
  }

  public int getButton() {
    return AInteractionsManager.LEFT;
  }

  public void start(Point2D.Double p, int region, int key) {}

  public void drag(Point2D.Double p, int region, int key) {
    AProjection proj=window.getProjection();

    if(proj instanceof AProjectionFR) {
      performFRSkew(-(p.y-p0.y)/p0.x, window);
    } else if(proj instanceof AProjectionFZ) {
      performFZSkew(-(p.y-p0.y)/p0.x, window);
    } else if(proj instanceof AProjectionRZ) {
      if(key==KeyEvent.VK_V)
        performFZSkew(-(p.y-p0.y)/p0.x, window);
      else
        performRZSkew(-(p.x-p0.x)/p0.y, window);
    }
  }

  public static void performFRSkew(double f, AWindow window) {
    Point2D.Double[] corners=window.getUserCorners();

    for(int i=0; i<corners.length; i++)
      corners[i].y+=f*corners[i].x;

    window.setUserCorners(corners);
  }

  public static void performFZSkew(double f, AWindow window) {
    Point2D.Double[] corners=window.getUserCorners();

    double zVtx=parameterStore.get("Event", "ZVtx").getD();

    for(int i=0; i<corners.length; i++)
      corners[i].y+=f*(corners[i].x-zVtx);

    window.setUserCorners(corners);
  }

  public static void performRZSkew(double f, AWindow window) {
    Point2D.Double[] corners=window.getUserCorners();

    for(int i=0; i<corners.length; i++)
      corners[i].x+=f*corners[i].y;

    window.setUserCorners(corners);
  }

  public void stop() {}

  public void cancel() {}

  public void paint(Graphics2D g) {}

  public int getPopupType() {
    return APopupListener.WINDOW_POPUP;
  }

  public JMenuItem[] getPopupItems() {
    return popupItems;
  }
  
  public void actionPerformed(ActionEvent e) {
    
  }
  public AModifier[] getModifiers(){
    if(window.getProjection() instanceof AProjectionRZ) {
      return new AModifier[] {
          new AModifier(KeyEvent.VK_V, false, "Y Skew"),
          new AModifier(KeyEvent.VK_H, false, "X Skew"),
          new AModifier(KeyEvent.VK_UNDEFINED, false, "X Skew")
      };
    } else {
      return new AModifier[0];
    }
    }


}
