package atlantis.canvas;

/**
 * Interface for all LayoutChangeListener classes. Each time the layout is 
 * changed, the layoutChanged method is called with the layout as an argument
 * @author maillard
 */
public interface ALayoutChangeListener {

  /**
   * This method gets called each time the layout is changed.
   * @param canvas the canvas
   */
  abstract public void layoutChanged(ACanvas canvas);  

}
