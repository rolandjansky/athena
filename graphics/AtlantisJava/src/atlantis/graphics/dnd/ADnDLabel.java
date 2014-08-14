package atlantis.graphics.dnd;

import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragSource;
import java.awt.dnd.DragSourceDragEvent;
import java.awt.dnd.DragSourceDropEvent;
import java.awt.dnd.DragSourceEvent;
import java.awt.dnd.DragSourceListener;
import java.awt.dnd.DropTarget;
import java.awt.dnd.DropTargetDragEvent;
import java.awt.dnd.DropTargetDropEvent;
import java.awt.dnd.DropTargetEvent;
import java.awt.dnd.DropTargetListener;
import java.util.Vector;

import javax.swing.JLabel;

import atlantis.graphics.ACursorFactory;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;

/**
 * A label which has Drag and Drop support.
 * Used in the Window Control.
 */
public class ADnDLabel extends JLabel
implements DropTargetListener, DragSourceListener, DragGestureListener {
	
	private DragSource dragSource=null;
	private Vector dragListeners;
	private boolean dragHighlight;
	private String[] page;
	
	public JLabel getLabel() {
		return this;
	}
	
	public static final Cursor DROP_VALID;
	public static final Cursor DROP_INVALID;
	
	static {
	/*	if(Toolkit.getDefaultToolkit().getBestCursorSize(64, 64).equals(new Dimension(64, 64))) {
			DROP_VALID=Toolkit.getDefaultToolkit().createCustomCursor(Toolkit.getDefaultToolkit().getImage(Atlantis.getHomeDirectory()
					+"img"+Atlantis.fileSep+"cursor_DnD_valid_64x64.gif"),
					new Point(1, 1), "ValidDnD");
			DROP_INVALID=Toolkit.getDefaultToolkit().createCustomCursor(Toolkit.getDefaultToolkit().getImage(Atlantis.getHomeDirectory()
					+"img"+Atlantis.fileSep+"cursor_DnD_invalid_64x64.gif"),
					new Point(9, 9), "InvalidDnD");
		} else {
			DROP_VALID=DragSource.DefaultMoveDrop;
			DROP_INVALID=DragSource.DefaultMoveNoDrop;
		} */
          DROP_VALID = ACursorFactory.getInstance().getDragValidCursor();
          DROP_INVALID = ACursorFactory.getInstance().getDragInvalidCursor();
	}
	
	public ADnDLabel(String name, boolean dragHighlight) {
		super(name, JLabel.CENTER);
		setName(name);
		setOpaque(true);
		page=null;
		this.dragHighlight=dragHighlight;
		
		dragSource=new DragSource();
		dragSource.createDefaultDragGestureRecognizer(this, DnDConstants.ACTION_MOVE, this);
        this.setDropTarget(new DropTarget(this, this));
		dragListeners=new Vector();
	}
	
	public ADnDLabel(String name, String[] wnames, boolean dragHighlight) {
		super(name, JLabel.CENTER);
		setName(name);
		setOpaque(true);
		page=wnames;
		this.dragHighlight=dragHighlight;
		
		dragSource=new DragSource();
		dragSource.createDefaultDragGestureRecognizer(this, DnDConstants.ACTION_MOVE, this);
        this.setDropTarget(new DropTarget(this, this));
		dragListeners=new Vector();
	}
	
	public Dimension getPreferredSize() {
		return new Dimension(15, 15);
	}
	
	// implementation of DropTargetListener
	
	private Color old;
	
	// Called when a drag operation has encountered the DropTarget.
	public void dragEnter(DropTargetDragEvent dtde) {
		dtde.acceptDrag(DnDConstants.ACTION_MOVE);
		if(dragHighlight) {
			old=getBackground();
			setBackground(new Color(150, 150, 150));
		}
	}
	
	// The drag operation has departed the DropTarget without dropping.
	public void dragExit(DropTargetEvent dte) {
		if(dragHighlight)
			setBackground(old);
	}
	
	// Called when a drag operation is ongoing on the DropTarget.
	public void dragOver(DropTargetDragEvent dtde) {
        }
	
	// The drag operation has terminated with a drop on this DropTarget.
	public void drop(DropTargetDropEvent dtde) {
		try {
			Transferable transferable=dtde.getTransferable();
			DataFlavor javaObjectDataFlavor=new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType);
			
			if(transferable.isDataFlavorSupported(javaObjectDataFlavor)) {
				dtde.acceptDrop(DnDConstants.ACTION_MOVE);
				Object object=transferable.getTransferData(javaObjectDataFlavor);
				
				if(object instanceof ACallBack)
					((ACallBack)object).callBack(this);
				else
					fireDragEvent(object);
				dtde.getDropTargetContext().dropComplete(true);
			} else
				AOutput.append("Cannot accept drop\n", ALogInterface.BAD_COMMAND);
		} catch(Exception e) {
			e.printStackTrace();
			dtde.rejectDrop();
		}
	}
	
	private void fireDragEvent(Object from) {
		for(int i=0; i<dragListeners.size(); i++)
			((ADragListener)dragListeners.get(i)).dragPerformed(from, this, -1);
	}
	
	// Called if the user has modified the current drop gesture.
	public void dropActionChanged(DropTargetDragEvent dtde) {}
	
	// implementation of DragSourceListener
	
	// This method is invoked to signify that the Drag and Drop operation is complete.
	public void dragDropEnd(DragSourceDropEvent dsde) {}
	
	// Called as the hotspot enters a platform dependent drop site.
	public void dragEnter(DragSourceDragEvent dsde) {
		dsde.getDragSourceContext().setCursor(DROP_VALID);
	}
	
	// Called as the hotspot exits a platform dependent drop site.
	public void dragExit(DragSourceEvent dse) {
		dse.getDragSourceContext().setCursor(DROP_INVALID);
	}
	
	// Called as the hotspot moves over a platform dependent drop site.
	public void dragOver(DragSourceDragEvent dsde) {
 		dsde.getDragSourceContext().setCursor(DROP_VALID);
	}
	
	// Called when the user has modified the drop gesture.
	public void dropActionChanged(DragSourceDragEvent dsde) {}
	
	// implementation of DragGestureListener
	public void dragGestureRecognized(DragGestureEvent dge) {
		dragSource.startDrag(dge, DROP_VALID, new AObjectTransferable(this), this);
	}
	
	public void addDragListener(ADragListener l) {
		dragListeners.addElement(l);
	}
	
	public String[] returnPageName()
	{
		return page;
	}
}
