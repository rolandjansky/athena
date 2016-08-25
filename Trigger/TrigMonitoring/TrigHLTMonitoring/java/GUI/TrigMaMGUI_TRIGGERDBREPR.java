////
//// 	GUI for TriggerHLTMonitoring MenuAwareMonitoring 
////	built by Xanthe Hoad (xhoad@cern.ch)
////

import java.awt.*;       
import java.awt.event.*; 
import javax.swing.*;    
import javax.swing.filechooser.*; 
import javax.swing.JFileChooser;
import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;
import javax.swing.text.*;
import java.io.File; 
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringWriter;
import java.io.Writer;
import java.io.Reader;
import java.util.concurrent.ExecutionException;
import java.util.List;
import java.lang.ProcessBuilder;

public class TrigMaMGUI_TRIGGERDBREPR extends JFrame {
    
    private int MCK_ID;
    private int SMK_ID;
    private int clone_MCK_ID;
    private String MCK_SMK_link_comment;
    private JTextArea GUI_messenger;
    private String jsonFilename;
    private File jsonSelectedFile;
    private String SMCK_list;
    private String clone_release;
    private String clone_version;
    
    static public Boolean print_to_terminal;
    
    public TrigMaMGUI_TRIGGERDBREPR () {
        
        print_to_terminal = true;
        
        //Set up
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);  // Exit program if close-window button clicked
        setTitle("Trigger MaM User Interface"); // "this" JFrame sets title
        setSize(520, 550);         // "this" JFrame sets initial size
        
        // Retrieve the content-pane of the top-level container JFrame
        // All operations done on the content-pane
        Container cp = getContentPane();
        cp.setLayout(new GridLayout(0, 1, 1, 1)); 
        
      	JPanel SMK_MCK_link_panel = new JPanel(new GridLayout(0, 1, 1, 1));
      	cp.add(SMK_MCK_link_panel);
        {
            JPanel SMK_MCK_header_panel = new JPanel();
            SMK_MCK_link_panel.add(SMK_MCK_header_panel);
            JLabel SMK_MCK_header = new JLabel("Make a new SMK-MCK link:");
            SMK_MCK_header_panel.add(SMK_MCK_header);
            
            JPanel SMK_MCK_link_subpanel_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
            SMK_MCK_link_panel.add(SMK_MCK_link_subpanel_1);
            SMK_MCK_link_subpanel_1.add(new JLabel("SMK ID:"));
      		final JTextField SMK_ID_field = new JTextField(5); 
      		SMK_MCK_link_subpanel_1.add(SMK_ID_field);	
            SMK_ID = 0; 
      		SMK_MCK_link_subpanel_1.add(new JLabel("MCK ID:"));
      		final JTextField MCK_ID_field = new JTextField(5); 
      		SMK_MCK_link_subpanel_1.add(MCK_ID_field);
            MCK_ID = 0; 
            
            SMK_MCK_link_subpanel_1.add(new JLabel("Comment:"));
      		final JTextArea linkComment = new JTextArea(""); 
      		linkComment.setEditable(true);
            linkComment.setLineWrap(true);
            linkComment.setWrapStyleWord(true);
      		JScrollPane scrollPane = new JScrollPane(linkComment); 
            scrollPane.setPreferredSize(new Dimension(200, 30));
            SMK_MCK_link_subpanel_1.add(scrollPane);
            JButton SMK_MCK_link_button = new JButton("Make SMK-MCK link");
            
            JPanel SMK_MCK_link_subpanel_2 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
            SMK_MCK_link_panel.add(SMK_MCK_link_subpanel_2);
            SMK_MCK_link_subpanel_2.add(SMK_MCK_link_button);
            final JCheckBox force_link_checkbox = new JCheckBox("Force link upload (only tick if certain you want to do this)");
            SMK_MCK_link_subpanel_2.add(force_link_checkbox);
            
            SMK_MCK_link_button.addActionListener(new ActionListener() {  
                public void actionPerformed(ActionEvent e) {	
                    Boolean GoodValsFlag = true; //we have good MCK and SMK values unless they are found to be bad    
                    
                    try { SMK_ID = Integer.parseInt(SMK_ID_field.getText().trim()); }
                    catch (NumberFormatException nfe) { GUI_messenger.append("\nBad SMK ID, must be an integer. "); GoodValsFlag = false; }
                    try { MCK_ID = Integer.parseInt(MCK_ID_field.getText().trim()); }
                    catch (NumberFormatException nfe) { GUI_messenger.append("\nBad MCK ID, must be an integer. "); GoodValsFlag = false; }
                    MCK_SMK_link_comment = linkComment.getText();
                    
                    if ( !MCK_SMK_link_comment.contains("\"\"\"") && !MCK_SMK_link_comment.contains("'''") && !MCK_SMK_link_comment.contains("'") && !MCK_SMK_link_comment.matches("(.*)\"(.*)") && GoodValsFlag == true ) {
                        GUI_messenger.append("\nSMK " + SMK_ID + " and MCK " + MCK_ID +" are both integers, as required.");
                        
                        if ( force_link_checkbox.isSelected() ) {
                            
                            Object[] options = {"Yes", "Cancel"};
                            int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to overide any existing links to: \nSMK ID:"+SMK_ID+" and create a link to MCK ID: "+MCK_ID+"? \nPlease be sure you want to do this. If you are unsure, click 'Cancel'.",
                                                                            "Confirm SMK-MCK link override",
                                                                            JOptionPane.YES_NO_OPTION,
                                                                            JOptionPane.PLAIN_MESSAGE,
                                                                            null, options, options[1]);
                            if (dialogResult == JOptionPane.YES_OPTION){
                                try {
                                    
                                    String link_command = "from MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;mam = MenuAwareMonitoringStandalone('TRIGGERDBREPR');mam.force_deactivate_all_links_for_smk("+Integer.toString(SMK_ID)+",True);";
                                    ProcessBuilder submit_link = new ProcessBuilder("python", "-c", link_command);
                                    
                                    Process submit_link_proc = submit_link.start();
                                    
                                    InputStream link_stdout = submit_link_proc.getInputStream();
                                    InputStream link_stderr = submit_link_proc.getErrorStream();
                                    Thread link_threadOut = new Thread( new MyInputStreamSink( link_stdout, "out", GUI_messenger ));
                                    Thread link_threadErr = new Thread( new MyInputStreamSink( link_stderr, "err", null ));
                                    link_threadOut.setDaemon(true);
                                    link_threadErr.setDaemon(true);
                                    link_threadOut.setName( String.format("link_stdout reader" ));
                                    link_threadErr.setName( String.format("link_stderr reader" ));
                                    link_threadOut.start();
                                    link_threadErr.start();
                                    
                                } catch (IOException err) {
                                }
                            }
                            else if (dialogResult == JOptionPane.NO_OPTION){
                                GUI_messenger.append("\nForce upload cancelled.");
                                force_link_checkbox.setSelected(false);
                            }
                            
                        }
                        Object[] options = {"Yes", "Cancel"};
                        int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to link: \nSMK ID:"+SMK_ID+" with MCK ID: "+MCK_ID+"\nwith comment: "+MCK_SMK_link_comment, 
                                                                        "Confirm SMK-MCK link", 
                                                                        JOptionPane.YES_NO_OPTION, 
                                                                        JOptionPane.PLAIN_MESSAGE, 
                                                                        null, options, options[1]);
                        if (dialogResult == JOptionPane.YES_OPTION){                        
                            try {                                
                                
                                String link_command = "from MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;mam = MenuAwareMonitoringStandalone('TRIGGERDBREPR');mam.link_smk_to_mck("+Integer.toString(SMK_ID)+","+Integer.toString(MCK_ID)+",'"+MCK_SMK_link_comment+"');";
                                ProcessBuilder submit_link = new ProcessBuilder("python", "-c", link_command);
                                
                                Process submit_link_proc = submit_link.start(); 
                                
                                InputStream link_stdout = submit_link_proc.getInputStream();
                                InputStream link_stderr = submit_link_proc.getErrorStream();
                                Thread link_threadOut = new Thread( new MyInputStreamSink( link_stdout, "out", GUI_messenger ));
                                Thread link_threadErr = new Thread( new MyInputStreamSink( link_stderr, "err", null ));
                                link_threadOut.setDaemon(true);
                                link_threadErr.setDaemon(true);
                                link_threadOut.setName( String.format("link_stdout reader" ));
                                link_threadErr.setName( String.format("link_stderr reader" ));
                                link_threadOut.start();
                                link_threadErr.start();
                                
                            } catch (IOException err) {
                            } 
                        }
                        else if (dialogResult == JOptionPane.NO_OPTION){                        
                            GUI_messenger.append("\nLink upload cancelled.");    
                            force_link_checkbox.setSelected(false);
                        }    
                    } else if ( MCK_SMK_link_comment.contains("\"") || MCK_SMK_link_comment.contains("\"\"\"") || MCK_SMK_link_comment.contains("'''") || MCK_SMK_link_comment.contains("'") || MCK_SMK_link_comment.matches("(.*)\"(.*)") ) {
                        GUI_messenger.append("\nComment contains \' or \", please remove these characters.");
                        force_link_checkbox.setSelected(false);
                    } }
            });
        }
        
        JPanel Make_MCK_panel = new JPanel(new GridLayout(0, 1, 1, 1));
        cp.add(Make_MCK_panel);
        {
            JPanel Make_MCK_header_panel = new JPanel();
            Make_MCK_panel.add(Make_MCK_header_panel);
            JLabel Make_MCK_header = new JLabel("Make a new MCK:");
            Make_MCK_header_panel.add(Make_MCK_header);
            
            JPanel Make_MCK_subpanel_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
            Make_MCK_panel.add(Make_MCK_subpanel_1);
            Make_MCK_subpanel_1.add(new JLabel("Comma seperated SMCK list:"));
            final JTextArea MCKlistField = new JTextArea(""); 
            MCKlistField.setEditable(true);
            MCKlistField.setLineWrap(true);
            MCKlistField.setWrapStyleWord(true);
            JScrollPane MCKlistScrollPane = new JScrollPane(MCKlistField); 
            MCKlistScrollPane.setPreferredSize(new Dimension(290, 30));
            Make_MCK_subpanel_1.add(MCKlistScrollPane);
            SMCK_list = "";
            
            JPanel Make_MCK_subpanel_2 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
            Make_MCK_panel.add(Make_MCK_subpanel_2);
            JButton Make_MCK_button = new JButton("Make new MCK");
            Make_MCK_subpanel_2.add(Make_MCK_button);
            
            Make_MCK_button.addActionListener(new ActionListener() { //Make MCK button pressed 
                public void actionPerformed(ActionEvent e) {
                    
                    SMCK_list = MCKlistField.getText().trim();
                    if ( SMCK_list.isEmpty() == true ) {
                        GUI_messenger.append("\nPlease add a comma seperated list of SMCKs.");
                    } else {
                        Object[] options = {"Yes",
                            "Cancel"};
                        int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to join the listed SMCKs?",
                                                                        "Confirm",
                                                                        JOptionPane.YES_NO_OPTION, 
                                                                        JOptionPane.PLAIN_MESSAGE, 
                                                                        null, options, options[1]);
                        if (dialogResult == JOptionPane.YES_OPTION){
                            try {
                                
                                String Make_MCK_command = "from MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;mam = MenuAwareMonitoringStandalone('TRIGGERDBREPR');mam.upload_mck(["+SMCK_list+"]);";  
                                ProcessBuilder make_mck = new ProcessBuilder("python", "-c", Make_MCK_command);
                                
                                Process make_mck_proc = make_mck.start();
                                
                                InputStream make_mck_stdout = make_mck_proc.getInputStream();
                                InputStream make_mck_stderr = make_mck_proc.getErrorStream();
                                Thread make_mck_threadOut = new Thread( new MyInputStreamSink( make_mck_stdout, "out", GUI_messenger ));
                                Thread make_mck_threadErr = new Thread( new MyInputStreamSink( make_mck_stderr, "err", null ));
                                make_mck_threadOut.setDaemon(true);
                                make_mck_threadErr.setDaemon(true);
                                make_mck_threadOut.setName( String.format("make_mck_stdout reader" ));
                                make_mck_threadErr.setName( String.format("make_mck_stderr reader" ));
                                make_mck_threadOut.start();
                                make_mck_threadErr.start();
                                
                            } catch (IOException err) {
                            }
                        }
                    } 
                }
                
            }); 
            
        }
        
        JPanel Clone_MCK_panel = new JPanel(new GridLayout(0, 1, 1, 1));
        cp.add(Clone_MCK_panel);
        {
            JPanel Clone_MCK_header_panel = new JPanel();
            Clone_MCK_panel.add(Clone_MCK_header_panel);
            JLabel Clone_MCK_header = new JLabel("Clone an MCK for use in a different release:");
            Clone_MCK_header_panel.add(Clone_MCK_header);
            
            JPanel Clone_MCK_subpanel_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
            Clone_MCK_panel.add(Clone_MCK_subpanel_1);
            Clone_MCK_subpanel_1.add(new JLabel("MCK ID:"));
      		final JTextField MCK_ID_field = new JTextField(5); 
      		Clone_MCK_subpanel_1.add(MCK_ID_field);	
            clone_MCK_ID = 0; 
      		Clone_MCK_subpanel_1.add(new JLabel("ATLAS project:"));
      		final JTextField release_field = new JTextField(8); 
      		Clone_MCK_subpanel_1.add(release_field);
            clone_release = ""; 
      		Clone_MCK_subpanel_1.add(new JLabel("ATLAS version:"));
      		final JTextField version_field = new JTextField(8); 
      		Clone_MCK_subpanel_1.add(version_field);
            clone_version = ""; 
            
            JPanel Clone_MCK_subpanel_2 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
            Clone_MCK_panel.add(Clone_MCK_subpanel_2);
            JButton Clone_MCK_button = new JButton("Clone MCK");
            Clone_MCK_subpanel_2.add(Clone_MCK_button);
            
            Clone_MCK_button.addActionListener(new ActionListener() { //Clone MCK button pressed 
                public void actionPerformed(ActionEvent e) {
                    
                    Boolean GoodValsFlag = true; //we have good MCK and SMK values unless they are found to be bad
                    try { clone_MCK_ID = Integer.parseInt(MCK_ID_field.getText().trim()); }
                    catch (NumberFormatException nfe) { GUI_messenger.append("\nBad MCK ID, must be an integer. "); GoodValsFlag = false; }
                    
                    clone_release = release_field.getText().trim();
                    clone_version = version_field.getText().trim();
                    if ( clone_release.isEmpty() == true || clone_version.isEmpty() ) {
                        GUI_messenger.append("\nPlease add the release and version that you want the new MCK to be valid in.");
                    } else {
                        if ( GoodValsFlag == true ) {
                            Object[] options = {"Yes",
                                "Cancel"};
                            int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to clone the entered MCK?",
                                                                            "Confirm",
                                                                            JOptionPane.YES_NO_OPTION, 
                                                                            JOptionPane.PLAIN_MESSAGE, 
                                                                            null, options, options[1]);
                            if (dialogResult == JOptionPane.YES_OPTION){
                                try {
                                    
                                    String Clone_MCK_command = "from MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;mam = MenuAwareMonitoringStandalone('TRIGGERDBREPR');mam.clone_mck_for_new_release("+clone_MCK_ID+",'"+clone_release+"','"+clone_version+"');";  
                                    ProcessBuilder clone_mck = new ProcessBuilder("python", "-c", Clone_MCK_command);
                                    
                                    Process clone_mck_proc = clone_mck.start();
                                    
                                    InputStream clone_mck_stdout = clone_mck_proc.getInputStream();
                                    InputStream clone_mck_stderr = clone_mck_proc.getErrorStream();
                                    Thread clone_mck_threadOut = new Thread( new MyInputStreamSink( clone_mck_stdout, "out", GUI_messenger ));
                                    Thread clone_mck_threadErr = new Thread( new MyInputStreamSink( clone_mck_stderr, "err", null ));
                                    clone_mck_threadOut.setDaemon(true);
                                    clone_mck_threadErr.setDaemon(true);
                                    clone_mck_threadOut.setName( String.format("clone_mck_stdout reader" ));
                                    clone_mck_threadErr.setName( String.format("clone_mck_stderr reader" ));
                                    clone_mck_threadOut.start();
                                    clone_mck_threadErr.start();
                                    
                                } catch (IOException err) {
                                }
                            }
                        } 
                    }
                }   
            }); 
            
        }
        
        JPanel Upload_json_panel = new JPanel(new GridLayout(0, 1, 1, 1));
        cp.add(Upload_json_panel);
        {
            JPanel json_header_panel = new JPanel();
            Upload_json_panel.add(json_header_panel);
            JLabel json_header = new JLabel("Upload a .json file to the database:");
            json_header_panel.add(json_header);
            
            JPanel Upload_json_subpanel_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
            Upload_json_panel.add(Upload_json_subpanel_1);
            Upload_json_subpanel_1.add(new JLabel(".json file to upload:"));
            final JFileChooser jsonChooser = new JFileChooser();
            final JTextField jsonFilenameField = new JTextField(32);
            jsonFilenameField.setEditable(false);
            Upload_json_subpanel_1.add(jsonFilenameField);
            
            JPanel Upload_json_subpanel_2 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
            Upload_json_panel.add(Upload_json_subpanel_2);
            JButton jsonChooserButton = new JButton("Choose file");
            Upload_json_subpanel_2.add(jsonChooserButton);
            final JButton submitFile = new JButton("Submit file");
            submitFile.setEnabled(false); //Cannot submit file until a file has been picked
            Upload_json_subpanel_2.add(submitFile);
            
            jsonChooserButton.addActionListener(new ActionListener() { //Choose file button pressed
                public void actionPerformed(ActionEvent e) {			
                    int chooserResult = jsonChooser.showOpenDialog((Component)e.getSource()); //Open file chooser
                    if (chooserResult == JFileChooser.APPROVE_OPTION) {
                        jsonSelectedFile = jsonChooser.getSelectedFile();
                        jsonFilename = jsonSelectedFile.getAbsolutePath();
                        jsonFilenameField.setText(jsonFilename+"\n");
                        submitFile.setEnabled(true);
                    }
                }
            });
            
            submitFile.addActionListener(new ActionListener() { //Submit file button pressed
                public void actionPerformed(ActionEvent e) {
                    
                    if (jsonSelectedFile.exists() && !jsonSelectedFile.isDirectory() /*&& release != null*/ ) {
                        String extension = jsonFilename.substring(jsonFilename.lastIndexOf("."),jsonFilename.length());
                        String file_with_extension = jsonFilename.substring(jsonFilename.lastIndexOf("/")+1,jsonFilename.length());
                        if (".json".equalsIgnoreCase(extension)) { 
                            GUI_messenger.append("\nGood file extension "+extension+".");
                            
                            Object[] options = {"Yes","Cancel"};
                            
                            int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to upload "+file_with_extension+" to the database?",
                                                                            
                                                                            "Confirm file upload",
                                                                            JOptionPane.YES_NO_OPTION, 
                                                                            JOptionPane.PLAIN_MESSAGE, 
                                                                            null, options, options[1]);
                            if (dialogResult == JOptionPane.YES_OPTION){
                                try {
                                    
                                    String json_command = "from MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;mam = MenuAwareMonitoringStandalone('TRIGGERDBREPR');mam.upload_config_from_json('"+jsonFilename+"');";  
                                    ProcessBuilder submit_json = new ProcessBuilder("python", "-c", json_command);                                  
                                    
                                    Process submit_json_proc = submit_json.start();
                                    
                                    InputStream json_stdout = submit_json_proc.getInputStream();
                                    InputStream json_stderr = submit_json_proc.getErrorStream();
                                    Thread json_threadOut = new Thread( new MyInputStreamSink( json_stdout, "out", GUI_messenger ));
                                    Thread json_threadErr = new Thread( new MyInputStreamSink( json_stderr, "err", null ));
                                    json_threadOut.setDaemon(true);
                                    json_threadErr.setDaemon(true);
                                    json_threadOut.setName( String.format("json_stdout reader" ));
                                    json_threadErr.setName( String.format("json_stderr reader" ));
                                    json_threadOut.start();
                                    json_threadErr.start();
                                    
                                } catch (IOException err) {
                                }
                            }
                            else if (dialogResult == JOptionPane.NO_OPTION){                        
                                GUI_messenger.append("\n"+file_with_extension+" upload cancelled.");    
                            }                            
                        }
                        else GUI_messenger.append("\nWrong file extension ("+extension+"): require .json.");		
                    }
                }
            });
        }
        
        JPanel Messenger_panel = new JPanel(new GridLayout(0, 1, 1, 1));
        cp.add(Messenger_panel);
        {
            GUI_messenger = new JTextArea(""); 
            GUI_messenger.setEditable(false);
            GUI_messenger.setLineWrap(true);
            GUI_messenger.setWrapStyleWord(true);
            JScrollPane messenger_scrollPane = new JScrollPane(GUI_messenger); 
            messenger_scrollPane.setPreferredSize(new Dimension(200, 30));
            DefaultCaret GUI_caret = (DefaultCaret)GUI_messenger.getCaret();
            GUI_caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
            Messenger_panel.add(messenger_scrollPane);
            
        }
        
    }
    
    private static class MyInputStreamSink implements Runnable {
        private InputStream m_in;
        private String m_streamName;
        private JTextArea m_outDest;
        
        MyInputStreamSink( InputStream in, String streamName, JTextArea outDest ) {
            m_in = in;
            m_streamName = streamName;
            m_outDest = outDest;
        }
        
        @Override
        public void run() {
            BufferedReader reader = null;
            Writer writer = null;
            
            try {
                if ( m_outDest != null ) {
                    String starting_process = "Starting process:";
                    m_outDest.append("\n\n"+starting_process);
                }
                
                reader = new BufferedReader( new InputStreamReader( m_in ) );
                
                for ( String line = null; ((line = reader.readLine()) != null); ) {
                    if ( print_to_terminal == true ) System.out.println(line);
                    if ( m_outDest != null ) {
                        if ( line.indexOf("Py:Athena") == -1 ) {
                            m_outDest.append("\n");
                            m_outDest.append(line);
                        }
                    }
                }
            } catch (IOException e) {
                System.out.println( "Unexpected I/O exception reading from process." );
            } /*catch (BadLocationException ble) {
               }*/
            finally {
                try {
                    if ( null != reader ) reader.close();
                }
                catch ( java.io.IOException e ) {
                    System.out.println( "Unexpected I/O exception closing a stream." );
                }
            }
        }
    }
    
    
    
    public static void main(String[] args) throws InterruptedException, IOException {
        
        // Run the GUI construction in the Event-Dispatching thread for thread-safety
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                try {
                    TrigMaMGUI_TRIGGERDBREPR frame = new TrigMaMGUI_TRIGGERDBREPR();
                    frame.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }
}
