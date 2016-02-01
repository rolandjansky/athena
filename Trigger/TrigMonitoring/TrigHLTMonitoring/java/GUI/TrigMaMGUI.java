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

public class TrigMaMGUI extends JFrame {
    
    private int MCK_ID;
    private int SMK_ID;
    private String MCK_SMK_link_comment;
    private JTextArea GUI_messenger;
    //private String release;
    private String jsonFilename;
    private File jsonSelectedFile;
    private String SMCK_list;
    //private String Username;
    //private String Password;
    //private String Database;
    
    static public Boolean print_to_terminal;
    
    public TrigMaMGUI () {
        
        //String AtlasPatchArea = System.getenv("AtlasPatchArea");
        //release = AtlasPatchArea.substring(AtlasPatchArea.lastIndexOf("/")+1,AtlasPatchArea.length());
        print_to_terminal = true;
        
        //Set up
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);  // Exit program if close-window button clicked
        setTitle("Trigger MaM User Interface"); // "this" JFrame sets title
        setSize(520, 550);         // "this" JFrame sets initial size
        
        // Retrieve the content-pane of the top-level container JFrame
        // All operations done on the content-pane
        Container cp = getContentPane();
        cp.setLayout(new GridLayout(0, 1, 1, 1)); 
        
        /*JPanel Login_panel = new JPanel(new GridLayout(0, 1, 1, 1));
      	cp.add(Login_panel);
        
        JPanel Login_header_panel = new JPanel();
        Login_panel.add(Login_header_panel);
        JLabel Login_header = new JLabel("Database login details:");
        Login_header_panel.add(Login_header);
        
        JPanel Login_subpanel_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
        Login_panel.add(Login_subpanel_1);
        Login_subpanel_1.add(new JLabel("Username:"));
        final JTextField username_field = new JTextField(8); 
        Login_subpanel_1.add(username_field);	
        Login_subpanel_1.add(new JLabel("Password:"));
        final JTextField password_field = new JTextField(8); 
        Login_subpanel_1.add(password_field);
        Login_subpanel_1.add(new JLabel("Database:"));
        final JTextField database_field = new JTextField(8); 
        Login_subpanel_1.add(database_field);*/
        
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
            
            SMK_MCK_link_button.addActionListener(new ActionListener() {  
                public void actionPerformed(ActionEvent e) {	
                    try { SMK_ID = Integer.parseInt(SMK_ID_field.getText().trim()); }
                    catch (NumberFormatException nfe) { GUI_messenger.append("\nBad SMK ID, must be an integer. "); SMK_ID = 0; }
                    try { MCK_ID = Integer.parseInt(MCK_ID_field.getText().trim()); }
                    catch (NumberFormatException nfe) { GUI_messenger.append("\nBad MCK ID, must be an integer. "); MCK_ID = 0; }
                    MCK_SMK_link_comment = linkComment.getText();
                    
                    //Username = username_field.getText().trim();
                    //Password = password_field.getText().trim();
                    //Database = database_field.getText().trim();

                    /*if ( Username.isEmpty() == true || Password.isEmpty() == true || Database.isEmpty() == true ) {
                        GUI_messenger.append("\nPlease add the database details.");
                    } else {*/
                    
                    if ( !MCK_SMK_link_comment.contains("\"\"\"") && !MCK_SMK_link_comment.contains("'''") && !MCK_SMK_link_comment.contains("'") && !MCK_SMK_link_comment.matches("(.*)\"(.*)") && SMK_ID != 0 && MCK_ID != 0 ) {
                        GUI_messenger.append("\nSMK " + SMK_ID + " and MCK " + MCK_ID +" are both integers, as required.");
                        
                        Object[] options = {"Yes", "Cancel"};
                        int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to link: \nSMK ID:"+SMK_ID+" with MCK ID: "+MCK_ID+"\nwith comment: "+MCK_SMK_link_comment, 
                                                                        "Confirm SMK-MCK link", 
                                                                        JOptionPane.YES_NO_OPTION, 
                                                                        JOptionPane.PLAIN_MESSAGE, 
                                                                        null, options, options[1]);
                        if (dialogResult == JOptionPane.YES_OPTION){                        
                            try {                                
                                //String link_command = "from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring;mam = MenuAwareMonitoring('"+Username+"','"+Password+"','"+Database+"');mam.upload_mck_to_smk_link("+Integer.toString(MCK_ID)+","+Integer.toString(SMK_ID)+",'"+MCK_SMK_link_comment+"');theApp.exit()";  
                                //ProcessBuilder submit_link = new ProcessBuilder("athena.py", "-i", "-c "+link_command);
                                
                                String link_command = "from MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;mam = MenuAwareMonitoringStandalone();mam.upload_mck_to_smk_link("+Integer.toString(MCK_ID)+","+Integer.toString(SMK_ID)+",'"+MCK_SMK_link_comment+"');";
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
                        }    
                    } else if ( MCK_SMK_link_comment.contains("\"") || MCK_SMK_link_comment.contains("\"\"\"") || MCK_SMK_link_comment.contains("'''") || MCK_SMK_link_comment.contains("'") || MCK_SMK_link_comment.matches("(.*)\"(.*)") ) {
                        GUI_messenger.append("\nComment contains \' or \", please remove these characters.");
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
                        //Username = username_field.getText().trim();
                        //Password = password_field.getText().trim();
                        //Database = database_field.getText().trim();
                        
                        /*if ( Username.isEmpty() == true || Password.isEmpty() == true || Database.isEmpty() == true ) {
                            GUI_messenger.append("\nPlease add the database details.");
                        } else {*/
                        Object[] options = {"Yes",
                            "Cancel"};
                        int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to join the listed SMCKs?",
                                                                        "Confirm file upload",
                                                                        JOptionPane.YES_NO_OPTION, 
                                                                        JOptionPane.PLAIN_MESSAGE, 
                                                                        null, options, options[1]);
                        if (dialogResult == JOptionPane.YES_OPTION){
                            try {
                                //String Make_MCK_command = "from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring;mam = MenuAwareMonitoring('"+Username+"','"+Password+"','"+Database+"');mam.upload_mck(["+SMCK_list+"]);theApp.exit()";  
                                //ProcessBuilder make_mck = new ProcessBuilder("athena.py", "-i", "-c "+Make_MCK_command+"");
                                
                                String Make_MCK_command = "from MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;mam = MenuAwareMonitoringStandalone();mam.upload_mck(["+SMCK_list+"]);";  
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
                        } //}
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
                            
                            //Username = username_field.getText().trim();
                            //Password = password_field.getText().trim();
                            //Database = database_field.getText().trim();
                            
                            /*if ( Username.isEmpty() == true || Password.isEmpty() == true || Database.isEmpty() == true ) {
                             GUI_messenger.append("\nPlease add the database details.");
                             } else {*/
                            
                            Object[] options = {"Yes","Cancel"};
                            //int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to upload "+file_with_extension+"? \nMake sure TrigMaMGUI is running within the release that the new config should be applied to. \nTrigMaMGUI is currently running in release: "+release+".",
                            int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to upload "+file_with_extension+" to the database?",

                                                                            "Confirm file upload",
                                                                            JOptionPane.YES_NO_OPTION, 
                                                                            JOptionPane.PLAIN_MESSAGE, 
                                                                            null, options, options[1]);
                            if (dialogResult == JOptionPane.YES_OPTION){
                                try {
                                    //String json_command = "from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring;mam = MenuAwareMonitoring('"+Username+"','"+Password+"','"+Database+"');mam.upload_config_from_json('"+jsonFilename+"');theApp.exit()";  
                                    //ProcessBuilder submit_json = new ProcessBuilder("athena.py", "-i", "-c "+json_command+"");
                                    
                                    String json_command = "from MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;mam = MenuAwareMonitoringStandalone();mam.upload_config_from_json('"+jsonFilename+"');";  
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
                            } //}                             
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
            
            //Highlighter.HighlightPainter cyanPainter = new DefaultHighlighter.DefaultHighlightPainter(Color.cyan);
            //Highlighter.HighlightPainter redPainter = new DefaultHighlighter.DefaultHighlightPainter(Color.red);  
            //Highlighter.HighlightPainter whitePainter = new DefaultHighlighter.DefaultHighlightPainter(Color.white);  
            
            //int endOfText = 0;
            //int lengthOfLine = 0;
            
            try {
                if ( m_outDest != null ) {
                    //if (  line.indexOf("_STATUS:") > -1 ) {
                    String starting_process = "Starting process:";
                    m_outDest.append("\n\n"+starting_process);
                    //endOfText = m_outDest.getText().length();  //NOT WORKING
                    //lengthOfLine = starting_process.length();
                    //System.out.println(Integer.toString(endOfText)+" "+Integer.toString(lengthOfLine));
                    //m_outDest.getHighlighter().addHighlight( endOfText-lengthOfLine, endOfText, cyanPainter);
                    //}
                }
                
                reader = new BufferedReader( new InputStreamReader( m_in ) );
                
                for ( String line = null; ((line = reader.readLine()) != null); ) {
                    if ( print_to_terminal == true ) System.out.println(line);
                    if ( m_outDest != null ) {
                        //if (  line.indexOf("_STATUS:") > -1 ) {
                        if ( line.indexOf("Py:Athena") == -1 ) {
                            m_outDest.append("\n");
                            m_outDest.append(line);
                            
                            /*if ( line.indexOf("Error") != -1 || line.indexOf("error") != -1 || line.indexOf("Attempting") != -1 ) {
                             System.out.println("^TRIGGERED");
                             endOfText = m_outDest.getDocument().getLength(); 
                             lengthOfLine = line.length();
                             m_outDest.getHighlighter().addHighlight( endOfText-lengthOfLine, endOfText, redPainter);
                             }*/ //Currently broken
                            
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
                    TrigMaMGUI frame = new TrigMaMGUI();
                    frame.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }
}
