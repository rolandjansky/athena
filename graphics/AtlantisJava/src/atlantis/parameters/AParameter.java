package atlantis.parameters;


import javax.swing.JComponent;


/**
 * This interface defines all the methods a parameter should have.
 */
public interface AParameter {
  // scope constants
  int LOCAL=1;
  int GLOBAL=2;

  // value type constants
  int NO_VALUE=0;
  int INT=1;
  int FLOAT=2;
  
  /** All supported supportedOperators */
  static final String[] supportedOperators= {"!=", ">=", ">", "<=", "<", "="};

  /** Initializes graphics for this parameter. */
  void initialize();

  /** Checks if the parameters graphics is initialized. */
  boolean isInitialized();

  /** Copies the value and status from one window to the other. */
  void copy(int from, int to);

  /** Copies the value and status from "window" in all the others. */
  void globalize(int window);

  /** Returns true if parameter is modulus, i.e. sign is irrelevant. */
  boolean isModulus();

  /** Return NO_VALUE, INT or FLOAT. */
  int getValueType();

  /** sets the scope to LOCAL or GLOBAL. */
  void setScope(int scope);

  /** Returns the scope of the parameter. */
  int getScope();

  /** Changes scope after it has originally been set. */
  void changeScope(int scope);

  /** Returns the name of the parameter. */
  String getName();

  /** returns the screen name of the parameter. */
  String getScreenName();

  /** Returns the tooltip text. */
  String getToolTip();

  /** Returns the current operator assigned to parameter. */
  String getOperator();

  /** Sets the double value for parameter. */
  void setD(double v);
  void setI(int v);

  /** Returns the value of the parameter. */
  double getD();
  /** Returns the value of the parameter. */
  int getI();
  /** Returns the value of the parameter. */
  String getValue();

  /** Sets the parameter's status. */
  void setStatus(boolean status);

  /** Returns the current status. */
  boolean getStatus();

  /** Returns the graphical component used to display the Name and Status of this parameter. */
  JComponent getNameComponent();

  /** Returns the graphical component used to display the value of this parameter. */
  JComponent getValueComponent();

  /** Make synchronisation between the value and status and graphical components. */
  void refresh();

  /** Returns true if "oper" represents an valid operator for this parameter. */
  boolean validateOperator(String oper);

  /** Returns true if "v" represents a valid value for this parameter. */
  boolean validateValue(double v);

  String getDifferences(String groupName);

  /** Validates and updates the current operator and value. */
  boolean processCommand(String oper, double v);

  /** Returns the user level associated with the parameter. */
  int getUserLevel();

  /** Returns a String representation of the parameter. */
  String toString();

  /** Add a parameter change listener. */
  void addParameterChangeListener(AParameterChangeListener listener);
}
