package atlantis.gui;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Font;
import java.awt.Frame;
import java.awt.Toolkit;
import java.util.ArrayList;
import java.util.EventObject;
import java.util.Iterator;
import java.util.Vector;

import javax.swing.JFrame;
import javax.swing.JTable;
import javax.swing.event.CellEditorListener;
import javax.swing.table.AbstractTableModel;
import javax.swing.ButtonGroup;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableModel;
import javax.swing.table.TableCellRenderer;
import javax.swing.JRadioButton;

import atlantis.canvas.ACanvas;
import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.interactions.AInteraction;
import atlantis.interactions.AModifier;

import java.awt.event.*;

/**
 * This dialog is used to display the mouse modifiers which are currently active
 * Once more a singleton, but at least a proper one (private constructor)
 */
public class AMouseHelpDialog extends JFrame {

    private static JTable table;
    static Object[][] rowData;
    static Object[] columnNames;
    private static AMouseHelpDialog instance;
    private static final AGlobals globals = AGlobals.instance();

    /**
     * Private singleton constructor
     */
    private AMouseHelpDialog() {
        super("Help - Modifier Keys");
        
        AIcon.setIconImage(this);
        
        table = new JTable() {
            public TableCellRenderer getCellRenderer(int row, int column) {
                if (column == 0) {
                    return new RadioRenderer();
                }
                // else...
                return super.getCellRenderer(row, column);
            }
            
            public TableCellEditor getCellEditor(int row, int column) {
                if (column == 0) {
                    return new RadioEditor();
                } else {
                    return super.getCellEditor(row, column);
                }
            }
        };
        
        table.setRowHeight(20);
        
        table.setFont(new Font("Monospaced", Font.PLAIN, table.getFont().getSize()));
        table.setRowSelectionAllowed(false);
        
        getContentPane().add(table.getTableHeader(), BorderLayout.NORTH);
        getContentPane().add(table, BorderLayout.CENTER);
        setResizable(false);
        
        // From Java 1.5 we can tell the window to be always on top
        this.setAlwaysOnTop(true);
        
        this.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                // Reset key modifier when closing the window
                AEventQueue.setDefault(AModifier.nothing);
            }
        });
        
        pack();

    }

    /**
     * Get an instance of this singleton
     * @return the AMouseHelpDialog instance
     */
    public static AMouseHelpDialog getInstance() {
      //Make sure no GUI object is created in headless mode
      if (AGlobals.isAtlantisHeadless()) return null;

      //Create instance if it does not exist
      if (instance == null) instance = new AMouseHelpDialog();
      return instance;
    }


    /**
     * Overwrite setVisible to ensure proper placement of the dialog
     * @param visible wether the dialog should be visible
     */
    @Override
    public void setVisible(boolean visible){

      //Set the proper location on the screen
      Frame gui = globals.getGuiFrame();
      int guiWidth = gui.getWidth();
      int guiX = gui.getX();
      int guiY = gui.getY();
      int dialogWidth = (int) instance.getPreferredSize().getWidth();
      int screenWidth = Math.round((float) Toolkit.getDefaultToolkit().getScreenSize().getWidth());
      if (guiX + guiWidth + (dialogWidth - guiWidth) / 2 > screenWidth)
          instance.setLocation(Math.max(0, screenWidth - dialogWidth), Math.max(0, guiY));
      else
          instance.setLocation(Math.max(0, guiX + (guiWidth - dialogWidth) / 2), Math.max(0, guiY));

      //And finally do set it visible!
      super.setVisible(visible);
    }
    
    public static void processInteractionChange(Vector interactions) {
        if (ACanvas.getCanvas().getCurrentWindow() == null)
            return;
        
        AModifier[] modifiers = ACanvas.getCanvas().getCurrentWindow().getInteractionManager().getMouseModifiers();
        ArrayList<AModifier> m = new ArrayList<AModifier>();
        for (int i = 0; i < modifiers.length; ++i)
            m.add(modifiers[i]);
        for (int i = 0; i < interactions.size(); ++i) {
            AModifier[] mod = ((AInteraction) interactions.get(i)).getModifiers();
            for (int j = 0; j < mod.length; ++j)
                m.add(mod[j]);
        }
        
        // remove duplicates
        for (int i = 0; i < m.size(); ++i)
            for (int j = i + 1; j < m.size(); ++j)
                if (m.get(i) != null && m.get(j) != null && ((AModifier)m.get(i)).sameAs((AModifier)m.get(j))) {
            m.set(i, null);
                }
        
        Iterator<AModifier> it = m.iterator();
        while (it.hasNext())
            if (it.next() == null)
                it.remove();
        
        // sort
        for (int i = 0; i < m.size(); ++i)
            for (int j = i + 1; j < m.size(); ++j) {
            String n = m.get(i).toKeyString();
            String o = m.get(j).toKeyString();
            if (o.length() > n.length() || (o.length() == n.length() && (o.length() > 0 && (o.charAt(0) < n.charAt(0))))) {
                AModifier temp = m.get(i);
                m.set(i, m.get(j));
                m.set(j, temp);
            }
            }
        
        int foundnull = -1;
        for (int i = 0; i < m.size(); i++) {
            AModifier mod = (AModifier)m.get(i);
            if (mod.sameAs(AModifier.nothing)) {
                foundnull = i;
            }
        }
        
        if (foundnull == -1) {
            m.add(AModifier.nothing);
        }
        
        ButtonGroup bgroup = new ButtonGroup();
        
        rowData = new Object[m.size()][3];
        for (int i = 0; i < rowData.length; ++i) {
            AModifier mod = (AModifier)m.get(i);
            JRadioButton but = new MouseHelpRadio(mod);

            if (mod.sameAs(AEventQueue.getDefault())) {
                but.setSelected(true);
            }
            
            if (mod.sameAs(AModifier.nothing)) {
                foundnull = i;
            }
            
            bgroup.add(but);
            
            rowData[i][0] = but;
            rowData[i][1] = mod.toKeyString();
            rowData[i][2] = mod.toDescString();
        }
        
        columnNames = new String[] { "", "Key", "Action" };
        TableModel model = new AbstractTableModel() {
            public String getColumnName(int column) {
                return columnNames[column].toString();
            }
            
            public int getRowCount() {
                return rowData.length;
            }
            
            public int getColumnCount() {
                return columnNames.length;
            }
            
            public Object getValueAt(int row, int col) {
                return rowData[row][col];
            }
            
            public boolean isCellEditable(int row, int column) {
                if (column == 0) { 
                    return true;
                } else {
                    return false;
                }
            }
            
            public void setValueAt(Object value, int row, int col) {
                rowData[row][col] = value;
                fireTableCellUpdated(row, col);
            }
        };
        
        table.setModel(model);
        table.getColumnModel().getColumn(0).setPreferredWidth(22);
        table.getColumnModel().getColumn(1).setPreferredWidth(80);
        table.getColumnModel().getColumn(2).setPreferredWidth(300);
        instance.pack();
    }

    private static class MouseHelpRadio extends JRadioButton {

    	AModifier m_mod;

    	public MouseHelpRadio(AModifier mod) {
    		m_mod = mod;

    		this.addActionListener(new ActionListener() {
    			public void actionPerformed(ActionEvent e) {
    				if (isSelected()) {
    					AEventQueue.setDefault(m_mod);
    				}
    				// Previously selected radio button doesn't always paint it's
    				// unselection if you aren't careful
    				getParent().repaint();
    			}
    		});
    	}

    }

    private static class RadioRenderer implements TableCellRenderer {

    	public RadioRenderer() {
    	}

    	public Component getTableCellRendererComponent(JTable table, Object value,
    			boolean isSelected, boolean hasFocus, int row, int column) {

    		return (JRadioButton)value;
    	}

    }

    private static class RadioEditor implements TableCellEditor {

    	private JRadioButton button;

    	public RadioEditor() {

    	}

    	public Object getCellEditorValue() {
    		return button;
    	}

    	public boolean shouldSelectCell(EventObject anEvent) {
    		return false;
    	}

    	public boolean isCellEditable(EventObject anEvent) {
    		return true;
    	}

    	public void removeCellEditorListener(CellEditorListener l) {
    	}

    	public void addCellEditorListener(CellEditorListener l) {
    	}

    	public boolean stopCellEditing() {
    		return true;
    	}

    	public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) {
    		button = (JRadioButton)value;
    		return button;
    	}

    	public void cancelCellEditing() {
    	}
    }
}