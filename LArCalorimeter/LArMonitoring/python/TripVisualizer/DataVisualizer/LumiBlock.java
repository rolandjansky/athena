public class LumiBlock implements Comparable
{
	final long time;
	final int LB;
  
  	public LumiBlock(int _LB, long _time) 
	{
		time = _time;		
		LB = _LB;
	};

	public String toString()
	{
		return ("LB: " + LB + " started at " + time);
	}

	public int compareTo(Object o)
	{
		if (o instanceof LumiBlock)
			return (int)((time - ((LumiBlock)o).time));
		else
			return 0;
	}

}
