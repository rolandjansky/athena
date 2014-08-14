package atlantis.gui;

import java.awt.Color;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;

import atlantis.output.ALogInterface;
import atlantis.utils.ALogger;

/**
 * This class is a subclass of JTextPane which can be used for any kind of
 * logging/text output window in Atlantis. It unifies the old AOutput and
 * ALogOutput classes
 *
 * @author Adam Davison
 */
public class ALogPane extends JScrollPane implements ALogInterface {

    private static ALogger logger = ALogger.getLogger(ALogPane.class);
    private Style m_systemDefaultStyle = null;
    
    private JTextPane m_textPane;
    private JPopupMenu m_popupMenu;
    private JMenuItem m_menuItemClearTextPane;
    
    public ALogPane() {
        m_textPane = new JTextPane();
        m_textPane.setEditable(false);
        m_textPane.setBackground(Color.white);
        
        setViewportView(m_textPane);
        setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        
        // popup menu at text pane (info output at the bottom of the GUI)
        m_popupMenu = new JPopupMenu();
        m_menuItemClearTextPane = new JMenuItem("Erase info output");
        m_menuItemClearTextPane.addActionListener(new PopupMenuActionListener(this));
        m_popupMenu.add(m_menuItemClearTextPane);
        
        m_textPane.addMouseListener(new PopupMenuListener_textPane(m_popupMenu));
        
        // create style which are used to add text to the document
        StyleContext context = StyleContext.getDefaultStyleContext();
        Style defaultStyle = context.getStyle(StyleContext.DEFAULT_STYLE);

        Style newStyle = m_textPane.addStyle(NORMAL, defaultStyle);

        StyleConstants.setFontFamily(newStyle, "Courier");
        StyleConstants.setFontSize(newStyle, 14);

        newStyle = m_textPane.addStyle(NORMAL_BOLD, defaultStyle);
        StyleConstants.setFontFamily(newStyle, "Courier");
        StyleConstants.setFontSize(newStyle, 14);
        StyleConstants.setBold(newStyle, true);

        newStyle = m_textPane.addStyle(COMMAND, defaultStyle);
        StyleConstants.setFontFamily(newStyle, "Courier");
        StyleConstants.setFontSize(newStyle, 14);
        StyleConstants.setForeground(newStyle, Color.blue);

        newStyle = m_textPane.addStyle(BAD_COMMAND, defaultStyle);
        StyleConstants.setFontFamily(newStyle, "Courier");
        StyleConstants.setFontSize(newStyle, 14);
        StyleConstants.setForeground(newStyle, Color.red);

        newStyle = m_textPane.addStyle(WARNING, defaultStyle);
        // was Courier - didn't print some greek letters under Linux
        StyleConstants.setFontFamily(newStyle, "Arial");
        StyleConstants.setFontSize(newStyle, 15);
        StyleConstants.setBold(newStyle, true);
        StyleConstants.setForeground(newStyle, Color.red);

        newStyle = m_textPane.addStyle(TITLE, defaultStyle);
        StyleConstants.setFontFamily(newStyle, "Courier");
        StyleConstants.setFontSize(newStyle, 16);
        StyleConstants.setItalic(newStyle, true);
        StyleConstants.setForeground(newStyle, Color.blue);

        newStyle = m_textPane.addStyle(PICK, defaultStyle);
        // was Courier - didn't print some greek letters under Linux
        StyleConstants.setFontFamily(newStyle, "Arial");

        StyleConstants.setFontSize(newStyle, 13);
        StyleConstants.setItalic(newStyle, true);
        StyleConstants.setForeground(newStyle, Color.blue);
        
        // This is the way ALogOutput styled it's text, perhaps we should
        // use NORMAL instead to unify the look of all the output boxes? - AD
        m_systemDefaultStyle = context.getStyle(StyleContext.DEFAULT_STYLE);
    }

    @Override
	public synchronized void append(String s) {
        append(s, m_systemDefaultStyle);
    }
    
    @Override
	public synchronized void append(String s, String style) {
        append(s, m_textPane.getStyle(style));
    }
    
    private synchronized void append(String s, Style style) {
        Document document = m_textPane.getDocument();
        try {
            int size = document.getLength();

            // if the string being appended gets too long, which happens in the
            // demo mode runs and leads to OutOfMemory here, cut off the first
            // half of the string in the information output
            if (size > 51200) // 50kB
            {
                document.remove(0, size / 2);

                // another option of cutting the string would be
                // (but it doesn't preserve styles):
                // String text = textPane.getText();
                // textPane.setText(""); // clear all
                // s = text.substring(size / 2, size - 1) + s;

                size = document.getLength();
            }

            document.insertString(size, s, style);

            // set the scrollbar to the bottom of the window
            // previous solution:
            // scrollPane.getViewport().setViewPosition(new Point(0, 9999999));
            // was sometimes crashing with IndexOutOfBoundException
            m_textPane.setCaretPosition(document.getLength());

            if (style.equals(ALogInterface.BAD_COMMAND) || style.equals(ALogInterface.WARNING)) {
                Toolkit.getDefaultToolkit().beep();
            }
        } catch (BadLocationException e) {
            logger.error("Problem in ALogPane", e);
        }
    }

    @Override
	public void clear() {
        m_textPane.setText("");
    }
    
}

class PopupMenuListener_textPane extends MouseAdapter
{
    private JPopupMenu popupMenu;

    PopupMenuListener_textPane(JPopupMenu popupMenu)
    {
        this.popupMenu = popupMenu;
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

} // class PopupMenuListener_textPane =======================================

class PopupMenuActionListener implements ActionListener
{
    ALogInterface adaptee;

    PopupMenuActionListener(ALogInterface adaptee)
    {
        this.adaptee = adaptee;
    }

    // clear info output if the popup menu item was pressed
    public void actionPerformed(ActionEvent e)
    {
        adaptee.clear();
    }

} // class PopupMenuActionListener ==========================================
