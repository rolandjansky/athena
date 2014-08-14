package atlantis.parameters;


import java.util.StringTokenizer;
import javax.swing.JComponent;


/**
 * A parameter which represents a link to another parameter.
 */
public class ALinkParameter implements AParameter {
  private String groupName, parameterName;
  private AParameter parameter=null;

  public ALinkParameter(String linkTo) {
    StringTokenizer st=new StringTokenizer(linkTo.trim(), ".");

    groupName=st.nextToken().trim();
    parameterName=st.nextToken().trim();
  }

  public void solveLink() {
    parameter=APar.instance().get(groupName, parameterName);
  }

  public void initialize() {
    parameter.initialize();
  }

  public boolean isInitialized() {
    return parameter.isInitialized();
  }

  public void copy(int from, int to) {
    parameter.copy(from, to);
  }

  public void globalize(int window) {
    parameter.globalize(window);
  }

  public boolean isModulus() {
    return parameter.isModulus();
  }

  public int getValueType() {
    return parameter.getValueType();
  }

  public void setScope(int scope) {}

  public void changeScope(int scope) {
    parameter.changeScope(scope);
  }

  public int getScope() {
    return parameter.getScope();
  }

  public String getName() {
    return parameter.getName();
  }

  public String getScreenName() {
      return parameter.getScreenName();
  }
  
  public String getToolTip() {
    return parameter.getToolTip();
  }

  public boolean validateOperator(String oper) {
    return parameter.validateOperator(oper);
  }

  public boolean validateValue(double v) {
    return parameter.validateValue(v);
  }

  public void setD(double v) {
    parameter.setD(v);
  }

  public void setI(int v) {
    parameter.setI(v);
  }

  public double getD() {
    return parameter.getD();
  }

  public int getI() {
    return parameter.getI();
  }

  public String getValue() {
    return parameter.getValue();
  }

  public String getCurrentText(){
      if(parameter instanceof AEnumeratorParameter)
          return ((AEnumeratorParameter) parameter).getCurrentText();
      else
          return null;
  }  
  
  public void setStatus(boolean status) {
    parameter.setStatus(status);
  }

  public boolean getStatus() {
    return parameter.getStatus();
  }

  public String toString() {
    return parameter.toString();
  }

  public String getOperator() {
    return parameter.getOperator();
  }

  public boolean processCommand(String oper, double value) {
    return parameter.processCommand(oper, value);
  }

  public JComponent getValueComponent() {
    return parameter.getValueComponent();
  }

  public JComponent getNameComponent() {
    return parameter.getNameComponent();
  }

  public void refresh() {
    parameter.refresh();
  }

  public int getUserLevel() {
    return parameter.getUserLevel();
  }

  public String getDifferences(String groupName) {
    return "";
  }

  @Override
  public void addParameterChangeListener(AParameterChangeListener listener) {
	  if (parameter!=null) parameter.addParameterChangeListener(listener);
  }
}
