package atlantis.utils;

/**
 * This class is used when clipping polygons.
 */
public class AClipPolygon extends APolygon {

  public AClipPolygon(double[] x, double[] y) {
    super(x, y);
    calc_hesse();
  }

  private double[][] normalized;
  private double[] distances;

  public double[] get_normalized(int index) {
    if(index>=0&&index<normalized.length&&normalized[index].length==3)
      return normalized[index];
    else
      return null;
  }

  public double get_distance(int index) {
    if(index>=0&&index<distances.length)
      return distances[index];
    else
      return Double.NaN;
  }

  public void calc_hesse() {
    int degree=nodes.size();

    // Check if fields are already allocated
    if(distances==null||distances.length!=degree)
      distances=new double[nodes.size()]; // Generate array for distances

    if(normalized==null||normalized.length!=degree)
      normalized=new double[nodes.size()][]; // array for normal vectors

    // Loop over all components and determine the normal vectors
    for(int i=0; i<degree; i++) {
      APoint r, next=null, current=null;

      if(nodes.elementAt(i) instanceof APoint)
        current=(APoint)nodes.elementAt(i);
      if(nodes.elementAt((i+1)%degree) instanceof APoint)
        next=(APoint)nodes.elementAt((i+1)%degree);

      // If both APoints exist, calculate the normalized vector
      if(next!=null&&current!=null) {

        normalized[i]=new double[3];
        r=next.minus(current);

        // Check values and determine normalized vector.
        if(r.x==0)
          if(r.y>0) {
            normalized[i][0]=1.0;
            normalized[i][1]=0.0;
            normalized[i][2]=1.0;
          } else if(r.y<0) {     // on the negative y-Axis
            normalized[i][0]=-1.0;
            normalized[i][1]=0.0;
            normalized[i][2]=1.0;
          }

        if(r.y==0)
          if(r.x>0) {       // on the positive x-Axis
            normalized[i][0]=0.0;
            normalized[i][1]=-1.0;
            normalized[i][2]=1.0;
          } else if(r.x<0) {    // on the negative x-Axis
            normalized[i][0]=0.0;
            normalized[i][1]=1.0;
            normalized[i][2]=1.0;
          }

        if(r.y!=0)
          if(r.x>0) {     // right of the y-Axis
            double x_val=r.y/r.x;
            double length=Math.sqrt(x_val*x_val+1);

            normalized[i][0]=x_val/length;
            normalized[i][1]=-1.0/length;
            normalized[i][2]=1.0;
          } else if(r.x<0) {   // left of the y-Axis
            double x_val=-r.y/r.x;
            double length=Math.sqrt(x_val*x_val+1);

            normalized[i][0]=x_val/length;
            normalized[i][1]=1.0/length;
            normalized[i][2]=1.0;
          }
        distances[i]=-current.dot_product(normalized[i]);
      }
    }
  }
}
