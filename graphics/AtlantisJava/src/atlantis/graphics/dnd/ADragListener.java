package atlantis.graphics.dnd;


/**
 * The listener to drag operations. It is suported by ADnDLabel and ADnDButton.
 */
public interface ADragListener {
  void dragPerformed(Object from, Object to, int action);
}
