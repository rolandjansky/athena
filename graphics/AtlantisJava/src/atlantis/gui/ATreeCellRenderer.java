package atlantis.gui;

import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;
import atlantis.parameters.AStatusParameter;
import hep.wired.util.TristateCheckBox;

public class ATreeCellRenderer implements TreeCellRenderer {
  
  public ATreeCellRenderer() {
  }
  
  public Component getTreeCellRendererComponent(JTree tree, Object value, boolean isSelected,
          boolean expanded, boolean leaf, int row, boolean hasFocus) {
    if(value instanceof ACheckNode){
      ACheckNode cellNode = (ACheckNode)value;
      cellNode.updateState();
      if (cellNode.getParameter() instanceof AStatusParameter){
        ACheckBox cellComponent = (ACheckBox)cellNode.getUserObject();
        cellComponent.setBorderPaintedFlat(true);
        return cellComponent;
      } else{
        TristateCheckBox cellComponent = (TristateCheckBox)cellNode.getUserObject();
        cellComponent.setState(cellNode.getState());
        cellComponent.setBorderPaintedFlat(true);
        return cellComponent;
      }
    }
    
    return new JLabel("not a checkNode");
  }
  
}
