package atlantis.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.LayoutManager;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.LookAndFeel;

import atlantis.utils.AUtilities;

/**
 * Used to display the status and the operator(<,>,=..etc) of a Cut Parameter.
 */
public class AMutableCheckBox extends JPanel {
	private JLabel label;
	private ACheckBox checkBox;
	private javax.swing.Timer timer;
	private JPopupMenu popup;
	private String selectedText;
	private ActionListener statusActionListener, textActionListener;
	private Vector oper=new Vector();
	
	public AMutableCheckBox(String name) {
		setLayout(new AMutableCheckBoxLayout());
		setBorder(null);
		
		checkBox=new ACheckBox(name);
        checkBox.setBackground(new Color(204, 204, 204));
		checkBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if(statusActionListener!=null)
					statusActionListener.actionPerformed(new ActionEvent(get(), 0, ""));
			}
		});
		add("CHECKBOX", checkBox);
		
		popup=new JPopupMenu();
	}
	
	// must be called after all the parameters whose added
	public void finalizeConstruction() {
		label=new JLabel("", JLabel.CENTER);
		label.setBorder(BorderFactory.createLineBorder(new Color(153, 153, 153), 1));
		LookAndFeel.installColorsAndFont(label, "CheckBox.background", "CheckBox.foreground",
		"CheckBox.font");
		add("LABEL", label);
		
		// setting the timmer and popup (if more than 2 supportedOperators)
		if(oper.size()>2) {
			timer=new javax.swing.Timer(50, new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					Dimension d=label.getSize();
					
					popup.show(label, 0, d.height);
				}
			});
			timer.setRepeats(false);
		}
		
		// setting the mouse listeners
		if(oper.size()<=2)
			label.addMouseListener(new MouseAdapter() {
				public void mousePressed(MouseEvent e) {
					if(AUtilities.isRightMouseButton(e)||oper.size()==1){
						return;
					}
					String text0=(String)oper.elementAt(0);
					String text1=(String)oper.elementAt(1);
					
					if(selectedText.equals(text0)) setSelectedText(text1);
					else if(selectedText.equals(text1)) setSelectedText(text0);
					
					if(textActionListener!=null)
						textActionListener.actionPerformed(new ActionEvent(get(), 0, ""));
				}
			});
		else
			label.addMouseListener(new MouseAdapter() {
				public void mousePressed(MouseEvent e) {
					if(!AUtilities.isRightMouseButton(e))
						timer.start();
				}
				
				public void mouseReleased(MouseEvent e) {
					if(!AUtilities.isRightMouseButton(e))
						timer.stop();
				}
			});
	}
	
	private JPanel get() {
		return this;
	}
	
	public ACheckBox getCheckBox(){
		return checkBox;
	}
	
	public void addItem(String s) {
		oper.addElement(s);
		
		popup.add(s).addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				JMenuItem item=(JMenuItem)e.getSource();
				
				setSelectedText(item.getText());
				if(textActionListener!=null)
					textActionListener.actionPerformed(new ActionEvent(get(), 0, ""));
			}
		});
	}
	
	public void setSelectedText(String text) {
		selectedText=text;
		label.setText(selectedText);
	}
	
	public String getSelectedText() {
		return selectedText;
	}
	
	public boolean getStatus() {
		return checkBox.isSelected();
	}
	
	public void setStatus(boolean status) {
		checkBox.setSelected(status);
	}
	
	public void addTextActionListener(ActionListener listener) {
		textActionListener=listener;
	}
	
	public void addStatusActionListener(ActionListener listener) {
		statusActionListener=listener;
	}
	
	public void setForeground(Color color) {
		if(checkBox!=null)
			checkBox.setForeground(color);
	}
}

class AMutableCheckBoxLayout implements LayoutManager {
	private Component checkBox, label;
	
	public void addLayoutComponent(String name, Component comp) {
		if(name.equals("CHECKBOX")) checkBox=comp;
		else if(name.equals("LABEL")) label=comp;
	}
	
	public void layoutContainer(Container parent) {
		Dimension p=parent.getSize();
		Dimension l=label.getPreferredSize();
		Dimension c=checkBox.getPreferredSize();
		
		checkBox.setBounds(0, (p.height-c.height)/2, p.width-l.width-10, c.height);
		label.setBounds(p.width-l.width-10, -1, l.width+10+1, p.height+2);
	}
	
	public Dimension minimumLayoutSize(Container parent) {
		return new Dimension(10, 10);
	}
	
	public Dimension preferredLayoutSize(Container parent) {
		return new Dimension(100, 100);
	}
	
	// Removes the specified component from the layout.
	public void removeLayoutComponent(Component comp) {}
}
