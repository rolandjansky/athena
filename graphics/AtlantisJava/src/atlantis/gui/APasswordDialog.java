package atlantis.gui;

import java.awt.Frame;
import java.awt.Container;
import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JPasswordField;
import javax.swing.JLabel;
import javax.swing.BoxLayout;

import java.net.PasswordAuthentication;
import java.net.Authenticator;

import atlantis.utils.ALogger;

/**
 * Provides a dialogue to prompt the user for a username and password for
 * reading events from a password-protected URL.
 * 
 * @author waugh
 */
public class APasswordDialog extends JDialog
{    
    private static final long serialVersionUID = 1L;

    private static ALogger logger = ALogger.getLogger(APasswordDialog.class);
    // input fields
    protected JTextField userField;
    protected JPasswordField passwordField;
    // authentication object containing username and password
    protected PasswordAuthentication pwAuth;
    
    /**
     * Create APasswordDialog belonging to given frame
     * @param frame owner of dialogue
     */
    protected APasswordDialog(Frame frame, String prompt) {
	// There is almost certainly a better way to do the layout...
	super(frame,"Password Dialogue",true);
	Container contentPane = getContentPane();

	// Create panels to place in content pane
	JPanel mainPanel = new JPanel();
	JPanel labelPanel = new JPanel();
	JPanel inputPanel = new JPanel();
	JPanel buttonPanel = new JPanel();

	// Create components (labels, inputs) to put in panels
	JLabel label = new JLabel(prompt);
	JLabel userLabel = new JLabel("User name");
	JLabel passwordLabel = new JLabel("Password");
	userField = new JTextField(15);
	passwordField = new JPasswordField(15);
	JButton okButton = new JButton("OK");
	JButton cancelButton = new JButton("Cancel");

	// Associate labels with corresponding input fields
	userLabel.setLabelFor(userField);
	passwordLabel.setLabelFor(passwordField);

	// Specify layout of dialogue
	contentPane.setLayout(new BoxLayout(contentPane,BoxLayout.Y_AXIS));
	mainPanel.setLayout(new BoxLayout(mainPanel,BoxLayout.X_AXIS));
	labelPanel.setLayout(new BoxLayout(labelPanel,BoxLayout.Y_AXIS));
	inputPanel.setLayout(new BoxLayout(inputPanel,BoxLayout.Y_AXIS));
	buttonPanel.setLayout(new BoxLayout(buttonPanel,BoxLayout.X_AXIS));

	// Put everything together: components in panels...
	contentPane.add(label);
	contentPane.add(mainPanel);	
	contentPane.add(buttonPanel);
	
	mainPanel.add(labelPanel);
	mainPanel.add(inputPanel);
	
	labelPanel.add(userLabel);
	labelPanel.add(passwordLabel);
	
	inputPanel.add(userField);
	inputPanel.add(passwordField);
	
	buttonPanel.add(okButton);
	buttonPanel.add(cancelButton);
	
	// click OK: construct authentication object from given name/password
	okButton.addActionListener(new ActionListener() 
	    {
		public void actionPerformed(ActionEvent e) {
		    String userName = userField.getText();
		    char[] password = passwordField.getPassword();
		    pwAuth = new PasswordAuthentication(userName,password);
		    dispose();
		}
	    }
	    );
	
	// click cancel: dispose of dialogue with no authentication object
	cancelButton.addActionListener(new ActionListener() 
	    {
		public void actionPerformed(ActionEvent e) {
		    pwAuth = null;
		    dispose();
		}
	    }
	    );		
	pack();

	// set position on screen (otherwise defaults to top left)
        int guiWidth = frame.getWidth();
        int guiHeight = frame.getHeight();
        int guiX = frame.getX();
        int dialogWidth = (int) this.getPreferredSize().getWidth();
        int dialogHeight = (int) this.getPreferredSize().getHeight();
        int screenWidth = Math.round((float) Toolkit.getDefaultToolkit().getScreenSize().getWidth());
        if(guiX+guiWidth+(dialogWidth-guiWidth)/2>screenWidth)
            this.setLocation(Math.max(0, screenWidth - dialogWidth), Math.max(0, (guiHeight - dialogHeight) / 3));
        else
            this.setLocation(Math.max(0, guiX + (guiWidth - dialogWidth) / 2), Math.max(0, (guiHeight - dialogHeight) / 3));

    }

    /**
     * Create APasswordDialog belonging to given frame, with default prompt
     * @param frame owner of dialogue
     */
    protected APasswordDialog(Frame frame) {
	this(frame,"Authentication required");
    }
    
    /** Gets authentication object
     * 
     * @return PasswordAuthentication object required by Authenticator
     */
    protected PasswordAuthentication getPasswordAuthentication() {
	return pwAuth;
    }
    
    /**
     * Displays username/password dialogue and returns authentication object. 
     * @param frame owner of dialogue, e.g. Atlantis GUI
     * @prompt prompt to show user
     * @return authentication object constructed from entered username/password,
     *         or null if dialogue cancelled
     */
    public static PasswordAuthentication showDialog(Frame frame, String prompt) {
	APasswordDialog d = new APasswordDialog(frame,prompt);
	d.setVisible(true);
	return d.getPasswordAuthentication();
    }
    
    /**
     * Displays username/password dialogue with default prompt and returns authentication object. 
     * @param frame owner of dialogue, e.g. Atlantis GUI
     * @return authentication object constructed from entered username/password,
     *         or null if dialogue cancelled
     */
    public static PasswordAuthentication showDialog(Frame frame) {
	APasswordDialog d = new APasswordDialog(frame);
	d.setVisible(true);
	return d.getPasswordAuthentication();
    }

    /**
     * Constructs and returns Authenticator object as required by
     * java.net.Authenticator.setDefault()
     * @param f owner of dialogue, e.g. Atlantis GUI
     * @return Authenticator that will create APasswordDialog when called
     */
    public static Authenticator getAuthenticator(Frame f) {
	return new PasswordAuthenticator(f);
    }
    
    /**
     * Private nested Authenticator subclass to be instantiated and returned
     * by getAuthenticator().
     * @author waugh
     */
    private static class PasswordAuthenticator extends Authenticator {
	protected Frame f;
	public PasswordAuthenticator(Frame frame) {f=frame;}
	protected PasswordAuthentication getPasswordAuthentication() {
	    logger.info("Authentication required: requesting host:"+getRequestingHost());
	    logger.info("Authentication required: requesting site:"+getRequestingSite());
	    logger.info("Authentication required: requesting protocol:"+getRequestingProtocol());
	    logger.info("Authentication required: requesting prompt:"+getRequestingPrompt());
	    logger.info("Authentication required: requesting scheme:"+getRequestingScheme());
	    //	    logger.info("Authentication required: requesting URL:"+getRequestingURL());
	    String prompt = "Authentication required by host "+getRequestingHost()+
		" with prompt \""+getRequestingPrompt()+"\"";
	    return APasswordDialog.showDialog(f,prompt);
	}
    }
}
