package atlantis.gui;

import java.util.Iterator;
import atlantis.parameters.AParameter;
import atlantis.parameters.AStatusParameter;
import atlantis.parameters.ACommandProcessor;
import hep.wired.util.DefaultTristateTreeNode;
import hep.wired.util.TristateCheckBox;

/**
 *
 * @author Qiang
 */

public class ACheckNode extends DefaultTristateTreeNode {
  
  public static final byte SINGLE_SELECTION = 0;
  
  public static final byte MULTI_SELECTION = 1;
  
  private byte selectionMode;
  
  private AParameter parameter;
  
  public ACheckNode(Object userObject, boolean selectionValue, byte selectionMode, AParameter parameter) {
    super(userObject, selectionValue, true);
    setSelectionMode(selectionMode);
    this.parameter = parameter;
  }
  
  public void setSelectionMode(byte selectionMode) {
    this.selectionMode = selectionMode;
  }
  
  public byte getSelectionMode() {
    return this.selectionMode;
  }
  
  public AParameter getParameter(){
    return this.parameter;
  }
  
  public void updateState(){
    if(children!=null){
      int checkedNumber = 0;
      for(Iterator it=children.iterator(); it.hasNext();){
        ACheckNode aChildNode = (ACheckNode)it.next();
        aChildNode.updateState();
        if(aChildNode.isSelected())
          checkedNumber++;
      }
      if(checkedNumber==0)
        this.setSelected(false);
      else
        this.setSelected(true);
    } else{
      ACheckBox cellComponent = (ACheckBox)getUserObject();
      cellComponent.setSelected(getParameter().getStatus());
      setSelected(cellComponent.isSelected());
    }
  }
  
  public void getClicked(boolean selectionFlag, boolean statusChange) {
    setSelected(selectionFlag);
    if((parameter instanceof AStatusParameter) && statusChange){
      // a leaf node is found here
      AStatusParameter theStatusParameter = (AStatusParameter)parameter;
      ACommandProcessor.receive(theStatusParameter.getName());
      theStatusParameter.apply();
    } else{
      // parameter is either AStatusRootParameter or AStatusGroupParameter
      if((selectionMode==MULTI_SELECTION) && (children!=null)){
        ((TristateCheckBox)this.getUserObject()).setSelected(selectionFlag);
        for(Iterator it=children.iterator(); it.hasNext();){
          ACheckNode aChildNode = (ACheckNode)it.next();
          // either AStatusGrooupParameter node or AStatusParameter node
          aChildNode.getClicked(selectionFlag, !(aChildNode.isSelected()==selectionFlag));
        }
      }
    }
    if(parent!=null){
      ((ACheckNode)parent).updateState();
    }
  }
  
}
