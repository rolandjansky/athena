package atlantis.interactions;


import java.awt.Point;


/**
 * The interactions implement this listener to get popup trigger events.
 */
public interface APopupListener {
  int REGION_POPUP=0;
  int WINDOW_POPUP=1;

  int getPopupType();
  void showPopup(Point p, int region);
}
