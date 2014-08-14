package atlantis.gui;

import java.awt.Color;
import java.awt.Frame;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.JFrame;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;

import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;

import java.awt.BorderLayout;
import java.awt.event.MouseListener;
import javax.swing.JLabel;


/**
 * The pointer postion window apears when selected in the preferences menu or
 * by using o+right click.
 * 
 * @author Sebastian Boeser
 */
public class APointerPositionWindow extends JFrame 
             implements WindowListener, ActionListener, MouseListener
             
{
    private static JLabel infoLabel;
    private static JPopupMenu popupMenu;
    private static JMenuItem menuItemClearTextPane;
    private static JTextPane textPane;
    private static JScrollPane scrollPane;
    private static APointerPositionWindow instance = null;
    private static final AGlobals globals = AGlobals.instance();

    public static APointerPositionWindow getInstance()
    {
        if (instance == null)
            instance = new APointerPositionWindow();
        return instance;
    }
    
    private APointerPositionWindow()
    {
	  	this.setTitle("Your Pointer Position");
	  	AIcon.setIconImage(this);
    
      //delete window if closed
      setDefaultCloseOperation(DISPOSE_ON_CLOSE);
      addWindowListener(this);
 
      //Tell user how to use this
      infoLabel = new JLabel();
      infoLabel.setText("To output your pointer position use: O + Right click\n");
        
      // popup menu at text pane (info output at bottom half of window)
      popupMenu = new JPopupMenu();
      menuItemClearTextPane = new JMenuItem("Erase output");
      menuItemClearTextPane.addActionListener(this);
      popupMenu.add(menuItemClearTextPane);
        
      // Text panel to show coordinates
      textPane = new JTextPane();
      textPane.setEditable(false);
      textPane.setBackground(Color.white);
      textPane.setText("Coordinates will be displayed here\n");
      textPane.addMouseListener(this);
        
      // Embedd in a scroll frame       
      scrollPane = new JScrollPane(textPane);
      scrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      scrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);

      add(infoLabel,BorderLayout.NORTH);
      add(scrollPane,BorderLayout.CENTER);
        
      // set the initial location
      Frame gui = globals.getGuiFrame();
      int guiWidth = gui.getWidth();
      int guiHeight = gui.getHeight();
      int guiX = gui.getX();
      int dialogWidth = (int) this.getPreferredSize().getWidth();
      int dialogHeight = (int) this.getPreferredSize().getHeight();
      int screenWidth = Math.round((float) Toolkit.getDefaultToolkit()
               .getScreenSize().getWidth());
      if (guiX + guiWidth + (dialogWidth - guiWidth) / 2 > screenWidth)
        this.setLocation(Math.max(0, screenWidth - dialogWidth), Math.max(
                    0, (guiHeight - dialogHeight) / 3));
      else
        this.setLocation(Math.max(0, guiX + (guiWidth - dialogWidth) / 2),
                    Math.max(0, (guiHeight - dialogHeight) / 3));
      //Layout the window
      pack();
    }
    
    public static void append(String s)
    {
        String text = textPane.getText();
        int size = text.length();
        //just as a precaution incase too many coordinates output
        if(size > 51200) // 50kB
        {
            textPane.setText(""); // clear all
            text = text.substring(size / 2, size - 1) + "\n";
        }
        s = text + "\n" + s;
        textPane.setText(s);
    }
    
    void clearTextPane(ActionEvent e)
    {
        textPane.setText("Coordinates will be displayed here\n");
    }
    
    public void dispose()
    {
        instance = null;
        super.dispose();
    }
    
    // clear info output if the popup menu item was pressed
    public void actionPerformed(ActionEvent e)
    {
      clearTextPane(e);
    }

    
    public void windowClosed(WindowEvent e)
    {
        // when this window is disposed, the menu item is switched off
        APreferencesControl.setPosMenuItem(false);
    }
    
    public void mousePressed(MouseEvent evt)
    {
        showPopupMenu(evt);
    }

    public void mouseReleased(MouseEvent evt)
    {
        showPopupMenu(evt);
    }

    private void showPopupMenu(MouseEvent evt)
    {
        // decides whether right 'trigger' action was performed to show
        // the popup menu (on the text pane - info output in GUI)
        if (evt.isPopupTrigger())
        {
            popupMenu.show(evt.getComponent(), evt.getX(), evt.getY());
        }
    }
    
    // Empty implementations of unused windows events
    public void windowOpened(WindowEvent e){} 
    public void windowClosing(WindowEvent e){}
    public void windowIconified(WindowEvent e){}
    public void windowDeiconified(WindowEvent e){}
    public void windowActivated(WindowEvent e){}
    public void windowDeactivated(WindowEvent e){}

    // Empty implementation of unused mouse events
    public void mouseClicked(MouseEvent e) {}
    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}
    
}
