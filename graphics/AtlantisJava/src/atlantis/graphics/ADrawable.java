package atlantis.graphics;


import atlantis.canvas.AWindow;
import atlantis.projection.AProjection2D;


public interface ADrawable {
  void draw(AWindow window, AGraphics ag, AProjection2D projection);
  int[] getColor(int[] dl);
  int[] getType(int[] dl);
  int getNumTypes();

  /**
   * The data are normally drawn in two layers
   *  (layer=0 is for frames)
   *  (layer=1 is the data)
   *  The data may be of more than 1 type which are drawn in sequence
   *   e.g. type=0 noise
   *        type=1 others
   */
  ADrawParameters getDrawParameters(int layer, int type);
}

