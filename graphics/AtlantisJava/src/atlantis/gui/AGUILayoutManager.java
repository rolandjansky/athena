package atlantis.gui;

import java.awt.Container;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.LayoutManager2;
import javax.swing.JComponent;
import javax.swing.JToolBar;

/**
 * The layout manager used by the GUI. It arranges the componets in a column.
 * The width is set to that of the window.
 * The height is set to the preffered height of the component.
 * One of the components will take all the remaining space.
 */
public class AGUILayoutManager implements LayoutManager2 {

  private final static int PxP=0;
  private final static int AxP=1;
  private final static int AxA=2;

  public final static Integer PREFERREDxPREFERRED=new Integer(PxP);
  public final static Integer AVAILABLExPREFERRED=new Integer(AxP);
  public final static Integer AVAILABLExAVAILABLE=new Integer(AxA);

  private final static String AGUI_LAYOUT_CONSTRAINT="AGUILayoutManager.StringConstraint";

  /**
   * Get the constraints registered for given component.
   */
  public Integer getConstraints(Component comp) {
    JComponent jc=(JComponent)comp;

    Integer constraint=(Integer)jc.getClientProperty(AGUI_LAYOUT_CONSTRAINT);

    return constraint;
  }

  public void addLayoutComponent(Component comp, Object constraint) {
    if(!(comp instanceof JComponent))
      throw new Error("AGUILayoutManager: not a JComponent");

    if(!(constraint instanceof Integer))
      throw new Error("AGUILayoutManager: not a Integer");

    JComponent jc=(JComponent)comp;

    jc.putClientProperty(AGUI_LAYOUT_CONSTRAINT, constraint);
  }

  public void layoutContainer(Container parent) {
    int W=parent.getSize().width;
    int H=parent.getSize().height;

    Component[] comp=parent.getComponents();
    int[] constraint=new int[comp.length];

    int AxA_Index=comp.length;

    for(int i=0; i<comp.length; i++) {
      constraint[i]=getConstraints(comp[i]).intValue();
      if(constraint[i]==AxA) AxA_Index=i;
    }

    int hi=0;

    for(int i=0; i<AxA_Index; i++) {
      comp[i].setSize(W, 0);
      Dimension cSize=comp[i].getPreferredSize();

      switch(constraint[i]) {
      case PxP:
        comp[i].setBounds((W-cSize.width)/2, hi, cSize.width, cSize.height);
        break;

      case AxP:
        comp[i].setBounds(0, hi, W, cSize.height);
        break;
      }
      hi+=cSize.height;
    }

    int hi2=H;

    for(int i=comp.length-1; i>AxA_Index; i--) {
      comp[i].setSize(W, 0);
      Dimension cSize=comp[i].getPreferredSize();

      hi2-=cSize.height;
      switch(constraint[i]) {
      case PxP:
        comp[i].setBounds((W-cSize.width)/2, hi2, cSize.width, cSize.height);
        break;

      case AxP:
        comp[i].setBounds(0, hi2, W, cSize.height);
        break;
      }
    }

    if(AxA_Index!=comp.length)
      comp[AxA_Index].setBounds(0, hi, W, hi2-hi);
  }

  public Dimension minimumLayoutSize(Container parent) {
    return new Dimension(0, 0);
  }

  public Dimension preferredLayoutSize(Container parent) {
    return new Dimension(0, 0);
  }

  public float getLayoutAlignmentX(Container target) {
    return 0.5f;
  }

  public float getLayoutAlignmentY(Container target) {
    return 0.5f;
  }

  public Dimension maximumLayoutSize(Container target) {
    return new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE);
  }

  public void addLayoutComponent(String name, Component comp) {}

  public void removeLayoutComponent(Component comp) {}

  public void invalidateLayout(Container target) {}

}
