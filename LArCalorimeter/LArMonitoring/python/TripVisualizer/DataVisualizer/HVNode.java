public class HVNode implements Comparable
{
	final long time;
	final int LB;
  	final float voltage;
  	final float current;
  	final int status;
  
  	public HVNode(	long _time,
			int _LB, 
			float _voltage, float _current,
			int _status)
  	{
		time = _time;		
		LB = _LB;
		voltage = _voltage;
 		current = _current;
		status = _status;

	};

	public String toString()
	{
		return ("Time: " + time + ", LB: " + LB + ", V: " + voltage + ", I: " + current + ", S: " + status);
	}
	public int compareTo(Object o)
	{
		if (o instanceof HVNode)
			return (int)((time - ((HVNode)o).time));
		else
			return 0;
	}

}
