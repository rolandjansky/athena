/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GUI for TriggerHLTMonitoring MenuAwareMonitoring
// Author: Xanthe Hoad (xanthe.hoad@cern.ch)
// See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

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
import java.util.ArrayList;
import java.util.Collections;
import java.lang.ProcessBuilder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class TrigMaMGUI extends JFrame {

  private int MCK_ID = 0;
  private int SMK_ID= 0;
  private String MCK_SMK_link_comment = "";
  private String Make_MCK_comment = "";
  private JTextArea GUI_messenger;
  private String jsonFilename = "";
  private File jsonSelectedFile;
  private String SMCK_list = "";
  private int clone_MCK_ID = 0;
  private String clone_release = "";
  private String clone_version = "";

  static private JComboBox<Integer> SMKbox = new JComboBox<Integer>();
  static private JComboBox<Integer> MCKbox = new JComboBox<Integer>();
  static private JComboBox<Integer> CloneMCKbox = new JComboBox<Integer>();
  static private JComboBox<Integer> ViewSMKbox = new JComboBox<Integer>();
  static private JComboBox<Integer> ViewMCKbox = new JComboBox<Integer>();
  static private JComboBox<Integer> ViewSMCKbox = new JComboBox<Integer>();
  static private String dbalias;
  static public Boolean print_to_terminal = false;

  public TrigMaMGUI() {

    //Set up
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);  // Exit program if close-window button clicked
    setTitle("Trigger Menu-aware Monitoring (MaM)");
    setSize(600, 600);
    setLocationByPlatform(true);
    setVisible(true);

    // Retrieve the content-pane of the top-level container JFrame
    // All operations done on the content-pane
    Container cp = getContentPane();
    //cp.setLayout(new GridLayout(0, 1, 1, 1));
    cp.setLayout(new GridBagLayout());
    GridBagConstraints c = new GridBagConstraints();
    c.fill = GridBagConstraints.BOTH;
    c.weightx = 1;

    // Link an MCK to an SMK
  	JPanel SMK_MCK_link_panel = new JPanel(new GridLayout(0, 1, 1, 1));
    {
      JPanel SMK_MCK_header_panel = new JPanel();
      SMK_MCK_link_panel.add(SMK_MCK_header_panel);
      JLabel SMK_MCK_header = new JLabel("Make a new SMK-MCK link");
      SMK_MCK_header_panel.add(SMK_MCK_header);

      JPanel SMK_MCK_link_subpanel_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      SMK_MCK_link_panel.add(SMK_MCK_link_subpanel_1);
      SMK_MCK_link_subpanel_1.add(new JLabel("SMK ID:"));
      RefreshComboBox( SMKbox, "smk" );
      SMK_MCK_link_subpanel_1.add(SMKbox);
      SMK_MCK_link_subpanel_1.add(new JLabel("MCK ID:"));
      RefreshComboBox( MCKbox, "mck" );
      SMK_MCK_link_subpanel_1.add(MCKbox);
      SMK_MCK_link_subpanel_1.add(new JLabel("Comment:"));
      final JTextArea linkComment = new JTextArea("");
      linkComment.setEditable(true);
      linkComment.setLineWrap(true);
      linkComment.setWrapStyleWord(true);
      JScrollPane scrollPane = new JScrollPane(linkComment);
      scrollPane.setPreferredSize(new Dimension(200, 30));
      SMK_MCK_link_subpanel_1.add(scrollPane);

      JPanel SMK_MCK_link_subpanel_2 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      SMK_MCK_link_panel.add(SMK_MCK_link_subpanel_2);
      JButton SMK_MCK_link_button = new JButton("Make SMK-MCK link");
      SMK_MCK_link_subpanel_2.add(SMK_MCK_link_button);
      final JCheckBox force_link_checkbox = new JCheckBox("Force link upload (only tick if you are certain you want to do this)");
      SMK_MCK_link_subpanel_2.add(force_link_checkbox);

      SMK_MCK_link_button.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {

          SMK_ID = (Integer)SMKbox.getSelectedItem();
          MCK_ID = (Integer)MCKbox.getSelectedItem();
          MCK_SMK_link_comment = linkComment.getText();

          if ( !MCK_SMK_link_comment.contains("\"\"\"") && !MCK_SMK_link_comment.contains("'''") && !MCK_SMK_link_comment.contains("'") && !MCK_SMK_link_comment.matches("(.*)\"(.*)") ) {

            if ( force_link_checkbox.isSelected() ) {
    			    Object[] options = {"Yes", "Cancel"};
    			    int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to overide any existing links to: \nSMK ID: "+SMK_ID+" and create a link to MCK ID: "+MCK_ID+"? \nPlease be sure you want to do this. If you are unsure, click 'Cancel'.",
          									    "Confirm SMK-MCK link override",
          									    JOptionPane.YES_NO_OPTION,
          									    JOptionPane.PLAIN_MESSAGE,
          									    null, options, options[1]);

              if (dialogResult == JOptionPane.YES_OPTION) {

                String link_command = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.force_deactivate_all_links_for_smk("+Integer.toString(SMK_ID)+",True);";

                RunProcess(link_command);

  			      } else if (dialogResult == JOptionPane.NO_OPTION) {
        				GUI_messenger.append("\nForce upload cancelled.");
        				force_link_checkbox.setSelected(false);
  			      }

  			    }

            Object[] options = {"Yes", "Cancel"};
            int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to link SMK ID: "+SMK_ID+" with MCK ID: "+MCK_ID+"\nwith comment: "+MCK_SMK_link_comment+"?",
                                                            "Confirm SMK-MCK link",
                                                            JOptionPane.YES_NO_OPTION,
                                                            JOptionPane.PLAIN_MESSAGE,
                                                            null, options, options[1]);
            if (dialogResult == JOptionPane.YES_OPTION) {

              String link_command = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.link_smk_to_mck("+Integer.toString(SMK_ID)+","+Integer.toString(MCK_ID)+",'"+MCK_SMK_link_comment+" ');";

              RunProcess(link_command);

            } else if (dialogResult == JOptionPane.NO_OPTION) {
              GUI_messenger.append("\nLink upload cancelled.");
              force_link_checkbox.setSelected(false);
  			    }
    			} else {
            GUI_messenger.append("\nComment contains \' or \", please remove these characters.");
    			  force_link_checkbox.setSelected(false);
          }
        }
      });
    }

    //Make an MCK from some existing SMCK
    JPanel Make_MCK_panel = new JPanel(new GridLayout(0, 1, 1, 1));
    {
      JPanel Make_MCK_header_panel = new JPanel();
      Make_MCK_panel.add(Make_MCK_header_panel);
      JLabel Make_MCK_header = new JLabel("Make an MCK from a list of SMCK");
      Make_MCK_header_panel.add(Make_MCK_header);

      JPanel Make_MCK_subpanel_1 = new JPanel(new BorderLayout());
      Make_MCK_panel.add(Make_MCK_subpanel_1);
      JPanel Make_MCK_subpanel_1_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      Make_MCK_subpanel_1_1.add(new JLabel("Comma seperated SMCK list:"));
      final JTextArea MCKlistField = new JTextArea("");
      MCKlistField.setEditable(true);
      MCKlistField.setLineWrap(true);
      MCKlistField.setWrapStyleWord(true);
      JScrollPane MCKlistScrollPane = new JScrollPane(MCKlistField);
      MCKlistScrollPane.setPreferredSize(new Dimension(290, 30));
      Make_MCK_subpanel_1_1.add(MCKlistScrollPane);
      JPanel Make_MCK_subpanel_1_2 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      Make_MCK_subpanel_1_2.add(new JLabel("Comment:"));
      final JTextArea MakeMCKcommentField = new JTextArea("");
      MakeMCKcommentField.setEditable(true);
      MakeMCKcommentField.setLineWrap(true);
      MakeMCKcommentField.setWrapStyleWord(true);
      JScrollPane Make_MCK_scrollPane = new JScrollPane(MakeMCKcommentField);
      Make_MCK_scrollPane.setPreferredSize(new Dimension(290, 30));

      Make_MCK_subpanel_1_2.add(Make_MCK_scrollPane);
      Make_MCK_subpanel_1.add(Make_MCK_subpanel_1_1, BorderLayout.NORTH);
      Make_MCK_subpanel_1.add(Make_MCK_subpanel_1_2, BorderLayout.SOUTH);

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
            Make_MCK_comment = MakeMCKcommentField.getText();
            if ( !Make_MCK_comment.contains("\"\"\"") && !Make_MCK_comment.contains("'''") && !Make_MCK_comment.contains("'") && !Make_MCK_comment.matches("(.*)\"(.*)") ) {
              Object[] options = {"Yes", "Cancel"};
              int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to join the listed SMCKs:\n"+SMCK_list+"\nwith comment: "+Make_MCK_comment,
                                                              "Confirm",
                                                              JOptionPane.YES_NO_OPTION,
                                                              JOptionPane.PLAIN_MESSAGE,
                                                              null, options, options[1]);
              if (dialogResult == JOptionPane.YES_OPTION) {

                  String Make_MCK_command = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.upload_mck(["+SMCK_list+"],'"+Make_MCK_comment+" ');";

                  RunProcess(Make_MCK_command);

              }
            }
          }
        }
      });
    }

    //Clone and MCK and assign a new release
    JPanel Clone_MCK_panel = new JPanel(new GridLayout(0, 1, 1, 1));
    {
      JPanel Clone_MCK_header_panel = new JPanel();
      Clone_MCK_panel.add(Clone_MCK_header_panel);
      JLabel Clone_MCK_header = new JLabel("Clone an existing MCK to use it in a different release");
      Clone_MCK_header_panel.add(Clone_MCK_header);

      JPanel Clone_MCK_subpanel_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      Clone_MCK_panel.add(Clone_MCK_subpanel_1);
      Clone_MCK_subpanel_1.add(new JLabel("MCK ID:"));
      RefreshComboBox( CloneMCKbox, "mck" );
      Clone_MCK_subpanel_1.add(CloneMCKbox);
      Clone_MCK_subpanel_1.add(new JLabel("ATLAS project:"));
      final JTextField release_field = new JTextField(8);
      Clone_MCK_subpanel_1.add(release_field);
      Clone_MCK_subpanel_1.add(new JLabel("ATLAS version:"));
      final JTextField version_field = new JTextField(8);
      Clone_MCK_subpanel_1.add(version_field);

      JPanel Clone_MCK_subpanel_2 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      Clone_MCK_panel.add(Clone_MCK_subpanel_2);
      JButton Clone_MCK_button = new JButton("Clone MCK");
      Clone_MCK_subpanel_2.add(Clone_MCK_button);

      Clone_MCK_button.addActionListener(new ActionListener() { //Clone MCK button pressed
        public void actionPerformed(ActionEvent e) {

          clone_MCK_ID = (Integer)CloneMCKbox.getSelectedItem();
          clone_release = release_field.getText().trim();
          clone_version = version_field.getText().trim();

          if ( clone_release.isEmpty() == true || clone_version.isEmpty() ) {
            GUI_messenger.append("\nPlease add the project and version that you want the new MCK to be valid in.");
          } else {
            Object[] options = {"Yes", "Cancel"};
            int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to clone MCK: "+clone_MCK_ID+"\nand assign the clone to be valid in: "+clone_release+"-"+clone_version+"?",
                                                            "Confirm",
                                                            JOptionPane.YES_NO_OPTION,
                                                            JOptionPane.PLAIN_MESSAGE,
                                                            null, options, options[1]);
            if (dialogResult == JOptionPane.YES_OPTION) {

              String Clone_MCK_command = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.clone_mck_for_new_release("+clone_MCK_ID+",'"+clone_release+"','"+clone_version+"');";

              RunProcess(Clone_MCK_command);

            } else if (dialogResult == JOptionPane.NO_OPTION) {
              GUI_messenger.append("\nCloning cancelled.");
            }
          }
        }
      });
    }

    //Upload SMCK and MCK to the DB from a json file
    JPanel Upload_json_panel = new JPanel(new GridLayout(0, 1, 1, 1));
    {
      JPanel json_header_panel = new JPanel();
      Upload_json_panel.add(json_header_panel);
      JLabel json_header = new JLabel("Upload a .json file to the database to create SMCK and MCK");
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

          if (jsonSelectedFile.exists() && !jsonSelectedFile.isDirectory()) {
            String extension = jsonFilename.substring(jsonFilename.lastIndexOf("."),jsonFilename.length());
            String file_with_extension = jsonFilename.substring(jsonFilename.lastIndexOf("/")+1,jsonFilename.length());
            if (".json".equalsIgnoreCase(extension)) {
              GUI_messenger.append("\nGood file extension "+extension+".");
              Object[] options = {"Yes", "Cancel"};

              int dialogResult = JOptionPane.showOptionDialog(null, "Are you sure you want to upload "+file_with_extension+" to the database?",
                                                              "Confirm file upload",
                                                              JOptionPane.YES_NO_OPTION,
                                                              JOptionPane.PLAIN_MESSAGE,
                                                              null, options, options[1]);
              if (dialogResult == JOptionPane.YES_OPTION) {

                String json_command = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.upload_config_from_json('"+jsonFilename+"');";

                RunProcess(json_command);

              } else if (dialogResult == JOptionPane.NO_OPTION) {
                GUI_messenger.append("\n"+file_with_extension+" upload cancelled.");
              }
            }
            else GUI_messenger.append("\nWrong file extension ("+extension+"): require .json.");
          }
        }
      });
    }

    //View SMK, MCK and SMCK
    JPanel View_keys_panel = new JPanel(new GridLayout(0, 1, 1, 1));
    {
      JPanel View_keys_header_panel = new JPanel();
      View_keys_panel.add(View_keys_header_panel);
      JLabel View_keys_header = new JLabel("View contents of keys");
      View_keys_header_panel.add(View_keys_header);

      JPanel View_keys_subpanel_1 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      View_keys_panel.add(View_keys_subpanel_1);
      View_keys_subpanel_1.add(new JLabel("SMK ID:"));
      RefreshComboBox( ViewSMKbox, "smk" );
      View_keys_subpanel_1.add(ViewSMKbox);
      View_keys_subpanel_1.add(new JLabel("MCK ID:"));
      RefreshComboBox( ViewMCKbox, "mck" );
      View_keys_subpanel_1.add(ViewMCKbox);
      View_keys_subpanel_1.add(new JLabel("SMCK ID:"));
      RefreshComboBox( ViewSMCKbox, "smck" );
      View_keys_subpanel_1.add(ViewSMCKbox);

      JPanel View_keys_subpanel_2 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      View_keys_panel.add(View_keys_subpanel_2);
      JButton ViewSMKbutton = new JButton("View SMK");
      View_keys_subpanel_2.add(ViewSMKbutton);
      JButton ViewMCKbutton = new JButton("View MCK");
      View_keys_subpanel_2.add(ViewMCKbutton);
      JButton ViewSMCKbutton = new JButton("View SMCK");
      View_keys_subpanel_2.add(ViewSMCKbutton);

      JPanel View_keys_subpanel_3 = new JPanel(new FlowLayout(FlowLayout.CENTER,1,1));
      View_keys_panel.add(View_keys_subpanel_3);
      JButton ViewLinksButton = new JButton("View all SMK-MCK links");
      View_keys_subpanel_3.add(ViewLinksButton);

      ViewSMKbutton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //Get the selected SMK info
          Integer view_SMK_ID = (Integer)ViewSMKbox.getSelectedItem();
          String viewSMKcommand = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.search('SMK_ID','"+view_SMK_ID+"');";
          RunProcess(viewSMKcommand);
        }
      });

      ViewMCKbutton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //Get the selected MCK info
          Integer view_MCK_ID = (Integer)ViewMCKbox.getSelectedItem();
          String viewMCKcommand = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.search('MCK_ID','"+view_MCK_ID+"');";
          RunProcess(viewMCKcommand);
        }
      });

      ViewSMCKbutton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //Get the selected MCK info
          Integer view_SMCK_ID = (Integer)ViewSMCKbox.getSelectedItem();
          String viewSMCKcommand = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.search('SMCK_ID','"+view_SMCK_ID+"');";
          RunProcess(viewSMCKcommand);
        }
      });

      ViewLinksButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //Get the selected MCK info
          String viewLinksCommand = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.print_all_mck_to_smk_links();";
          RunProcess(viewLinksCommand);
        }
      });
    }

    //Setting up the tabbed pane
    JTabbedPane tabbedPane = new JTabbedPane();
    //Add the tabbed pane to the container.
    c.weighty = 0.2;
    c.gridy = 0;
    c.anchor = GridBagConstraints.PAGE_START;
    cp.add(tabbedPane,c);
    //The following line enables to use scrolling tabs.
    tabbedPane.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);

    tabbedPane.addTab("MCK-SMK link", null, SMK_MCK_link_panel, "Link an MCK to an SMK");
    tabbedPane.setMnemonicAt(0, KeyEvent.VK_1);

    tabbedPane.addTab("Make MCK", null, Make_MCK_panel, "Make an MCK from a list of SMCK");
    tabbedPane.setMnemonicAt(1, KeyEvent.VK_2);

    tabbedPane.addTab("Clone MCK", null, Clone_MCK_panel, "Clone an MCK and assign it to a different release");
    tabbedPane.setMnemonicAt(2, KeyEvent.VK_3);

    tabbedPane.addTab("Upload .json", null, Upload_json_panel, "Upload a .json file to the DB to create SMCKs (and an MCK linking all uploaded SMCK)");
    tabbedPane.setMnemonicAt(3, KeyEvent.VK_4);

    tabbedPane.addTab("View keys", null, View_keys_panel, "View the contents of SMK, MCK and SMCK");
    tabbedPane.setMnemonicAt(4, KeyEvent.VK_5);

    //Refresh the combobox lists whenever the tab is changed to a tab with a combobox
    ChangeListener changeListener = new ChangeListener() {
      public void stateChanged(ChangeEvent changeEvent) {
        JTabbedPane sourceTabbedPane = (JTabbedPane) changeEvent.getSource();
        int index = sourceTabbedPane.getSelectedIndex();
        //Refresh the combobox lists when we change to relevant tabs
        if (index == 0) {
          //MCK-SMK link panel
          RefreshComboBox( SMKbox, "smk" );
          RefreshComboBox( MCKbox, "mck" );
        } else if (index == 2) {
          //Clone MCK panel
          RefreshComboBox( CloneMCKbox, "mck" );
        } else if (index == 4) {
          //View keys panel
          RefreshComboBox( ViewSMKbox, "smk" );
          RefreshComboBox( ViewMCKbox, "mck" );
          RefreshComboBox( ViewSMCKbox, "smck" );
        }

      }
    };
    tabbedPane.addChangeListener(changeListener);

    //Setting up the messenger pane
    //Static panel under the tabs
    JPanel Messenger_panel = new JPanel(new GridLayout(0, 1, 1, 1));
    c.weighty = 0.8;
    c.gridy = 1;
    c.anchor = GridBagConstraints.PAGE_END;
    cp.add(Messenger_panel,c);
    {
        GUI_messenger = new JTextArea("");
        GUI_messenger.setEditable(false);
        GUI_messenger.setLineWrap(true);
        GUI_messenger.setWrapStyleWord(true);
        JScrollPane messenger_scrollPane = new JScrollPane(GUI_messenger);
        //messenger_scrollPane.setPreferredSize(new Dimension(200, 30));
        DefaultCaret GUI_caret = (DefaultCaret)GUI_messenger.getCaret();
        GUI_caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
        Messenger_panel.add(messenger_scrollPane);
    }

  }

  private void RunProcess(String command) {
    try {

        GUI_messenger.append("\nReceived command:\n"+command+"\n");

        ProcessBuilder procbuild = new ProcessBuilder("python", "-c", command);

        Process proc = procbuild.start();

        InputStream stdout = proc.getInputStream();
        InputStream stderr = proc.getErrorStream();
        Thread threadOut = new Thread( new MyInputStreamSink( stdout, "out", GUI_messenger ));
        Thread threadErr = new Thread( new MyInputStreamSink( stderr, "err", GUI_messenger ));
        threadOut.setDaemon(true);
        threadErr.setDaemon(true);
        threadOut.setName( String.format("stdout reader" ));
        threadErr.setName( String.format("stderr reader" ));
        threadOut.start();
        threadErr.start();

        GUI_messenger.append("\n");

    } catch (IOException err) {
    }
  }

  private static ArrayList<Integer> GetList( String dbalias, String keytype ) {

    ArrayList<Integer> results = new ArrayList<Integer>();
    String command = "from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone;ms = MenuAwareMonitoringStandalone('"+dbalias+"');ms.oi.get_"+keytype+"_list()";

    try {
      ProcessBuilder procbuild = new ProcessBuilder("python", "-c", command);
      Process proc = procbuild.start();
      BufferedReader reader = new BufferedReader(new InputStreamReader(proc.getInputStream()));
      String line = null;
      int linetemp = 0;
      while ( (line = reader.readLine()) != null ) {
        try {
          linetemp = Integer.parseInt(line.trim());
          results.add(linetemp);
          //System.out.println(line);
        } catch ( NumberFormatException nfe ) {
          //System.out.println("Not an int:");
          //System.out.println(line);
        }
      }
    } catch ( IOException e ) {
      e.printStackTrace();
      return results;
    }
    Collections.reverse(results);
    return results;
  }

  public void RefreshComboBox( JComboBox<Integer> combobox, String keytype ) {

    //Create and update the list of either SMK or MCK
    ArrayList<Integer> menulist = new ArrayList<Integer>();
    menulist = GetList( dbalias, keytype );
    //Set the comboboxmodel, updating the dropdown list
    combobox.setModel(new DefaultComboBoxModel<Integer>(menulist.toArray(new Integer[menulist.size()])));
  }

  public static void main(String[] args) throws InterruptedException, IOException {
    // Run the GUI construction in the Event-Dispatching thread for thread-safety
    SwingUtilities.invokeLater(new Runnable() {
      @Override
      public void run() {
        if (args.length == 0) {
          // cannot start the GUI if the dbstring has not been provided
          // should be "", "TRIGGERDB" or "TRIGGERDBREPR"
          System.out.println("Cannot open the GUI without the DB alias to us.");
          System.out.println("Please provide the DB alias as a command line arg.");
          System.exit(1);
        } else {
            try {

              dbalias = args[0];
              if (args.length > 1) {
                print_to_terminal = args[1].equalsIgnoreCase("true") || args[1].equalsIgnoreCase("t") || args[1].equalsIgnoreCase("yes") || args[1].equalsIgnoreCase("y") || args[1].equalsIgnoreCase("print") || args[1].equalsIgnoreCase("print_to_terminal") || args[1].equalsIgnoreCase("1");
              }
              TrigMaMGUI frame = new TrigMaMGUI();

            } catch (Exception e) {
              e.printStackTrace();
            }
        }
      }
    });
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
          //String starting_process = "Starting process:";
          //m_outDest.append("\n\n"+starting_process);
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
        } catch ( java.io.IOException e ) {
          System.out.println( "Unexpected I/O exception closing a stream." );
        }
      }
    }
  }
}
