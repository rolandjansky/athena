package atlantis.interactions;


import java.awt.geom.*;


/**
 * Implemented by the interactions to get press events
 * from the InteractionsManager.
 */
public interface AMousePressListener {

  int getPressButton();
  void pressed(Point2D.Double p, int button, int key);
}

