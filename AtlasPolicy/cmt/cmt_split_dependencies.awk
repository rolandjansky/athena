BEGIN {print "" > output;}
{
  if (debug == "1")
    {
      print "--------------------";
      print "$1=[" $1 "]";
      print "$NF=[" $NF "]";
    }

  stamp = "$(bin)" CONSTITUENT "/" $1 ".stamp";
  newstampfile = bin "/" CONSTITUENT "/" $1 ".new";
  stampfile = bin "/" CONSTITUENT "/" $1 ".stamp";

  if (debug == "1")
    {
      print "stamp=[" stamp "]";
    }

  getline olddeps < stampfile
  if (debug == "1")
    {
      print "olddeps=[" olddeps "]";
    }

  newdeps = $0 " " stamp;
  if (debug == "1")
    {
      print "newdeps=[" newdeps "]";
    }

  if (olddeps == "")
    {
      print newdeps > stampfile
    }
  else if (olddeps != newdeps)
    {
      print newdeps > stampfile
    }
  print newdeps >> output;
}
