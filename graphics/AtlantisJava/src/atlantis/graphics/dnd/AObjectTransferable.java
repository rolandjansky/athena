package atlantis.graphics.dnd;


import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;


/**
 * Used by Drag and Drop
 * A implementation of a transferable which
 * transfers an Java object (really an pointer to it).
 */
public class AObjectTransferable implements Transferable {
  private Object object;

  /**
   * Creates the Transferable which will transfer the specified object.
   * @param object The object to be transferred
   */
  public AObjectTransferable(Object object) {
    this.object=object;
  }

  /**
   * Returns the transferred for a specific DataFlavor.
   * @param flavor The DataFlavor for which to get the transferred object.
   * @return The transferred object.
   */
  public Object getTransferData(DataFlavor flavor) {
    return object;
  }

  /**
   * Returns an array of DataFlavor objects indicating the flavors the
   * data can be provided in.
   * @return The set of available flavors.
   */
  public DataFlavor[] getTransferDataFlavors() {
    DataFlavor df=null;

    try {
      df=new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType);
    } catch(ClassNotFoundException e) {
      throw new Error("Cannot create DataFlavor.javaJVMLocalObjectMimeType "+e.getMessage());
    }
    return new DataFlavor[] {df};
  }

  /**
   * Returns whether or not the specified data flavor is
   * supported by this transferable.
   * @param flavor The data flavor that needs to be checked
   * @return True if the flavor is supported. False Otherwise.
   */
  public boolean isDataFlavorSupported(DataFlavor flavor) {
    if(flavor.getHumanPresentableName().equals(DataFlavor.javaJVMLocalObjectMimeType))
      return true;
    else
      return false;
  }

}
