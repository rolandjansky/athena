class Vec2:
    def __init__(self, l0, l1):
        self.l0 = l0
        self.l1 = l1
    
    @property
    def x(self):
        return self.l0
    
    @property
    def y(self):
        return self.l1
        
    @property
    def norm(self):
        return self.r
        
    @property
    def phi(self):
        return atan2(self.y, self.x)
    
    @property
    def r(self):
        return sqrt(self.x**2 + self.y**2)
    
    @property
    def xy(self):
        return (self.l0, self.l1)
    
    @property
    def ixy(self):
        return (self.l1, self.l0)
    
    def __mul__(self, other):
        return Vec2(self.l0*other, self.l1*other)
    
    def __rmul__(self, other):
        return self.__mul__(other)
    
    def __truediv__(self, other):
        return Vec2(self.l0/other, self.l1/other)
    
    def __rtruediv__(self, other):
        return self.__div__(other)
    
    def __getitem__(self, key):
        if key == 0: return self.l0
        else: return self.l1
    
    def __add__(self, other):
        return Vec2(self.l0 + other.l0, self.l1 + other.l1)
        
    def __sub__(self, other):
        return Vec2(self.l0 - other.l0, self.l1 - other.l1)
        
    def __repr__(self):
        return "Vec2(l0=%.3f, l1=%.3f)" % (self.l0, self.l1)
        
    def __iter__(self):
        return iter((self.l0, self.l1))
        
    def to_cartesian(self):
        # are polar, will be cartesian
        return Vec2(self.l0*cos(self.l1), self.l0*sin(self.l1))
    
    def to_polar(self):
        # are cart, will be polar
        return Vec2(self.r, self.phi)
    
    def normalized(self):
        return Vec2(self.l0, self.l1) / self.norm
    
    @property
    def xy(self):
        return (self.l0, self.l1)
    
class Line2:
    def __init__(self, a, b):
        self.a = a
        self.b = b
        self.d = b - a
    
    @property
    def xy(self):
        f = np.linspace(0, 1, 10)
        l0 = (self.d*f + self.a).l0
        l1 = (self.d*f + self.a).l1
        return (l0, l1)
    
class LineArray:
    def __init__(self, lines):
        if type(lines) == list:
            self.lines = lines
        else:
            self.lines = lines.split("\n")
        
    @classmethod
    def fromstring(cls, s):
        return cls(s.split("\n"))
    
    @classmethod
    def center(cls, s, n):
        assert type(s) == str
        lines = [" "*len(s)]*n
        lines[n//2] = s
        return cls(lines)
    
    
    def prefix(self, s):
        return self.__class__([s]*len(self)) + self
    
    def postfix(self, s):
        return self + self.__class__([s]*len(self))
    
    def __and__(self, other):
        assert type(other) == str
        return self.postfix(other)
    
    def __rand__(self, other):
        assert type(other) == str
        return self.prefix(other)
    
    def _pad(self, a, b):
        if len(a) == len(b):
            return a, b
        #smaller = min(a, b, key=lambda e: len(e))
        #bigger = max(a, b, key=lambda e: len(e))
        if len(a) > len(b):
            smaller = b
            bigger = a
            order = 0
        else:
            smaller = a
            bigger = b
            order = 1
        
        pad = len(bigger) - len(smaller)
        top = pad // 2
        bottom = pad - top
        
        assert top + len(smaller) + bottom == len(bigger)
        
        padstr = " "*len(max(smaller, key=lambda s: len(s)))
        
        smaller_padded = [padstr]*top + smaller + [padstr]*bottom
        
        if order == 0:
            return bigger, smaller_padded
        else:
            return smaller_padded, bigger
    
    def __add__(self, other):
        if type(other) == list:
            #assert len(self.lines) == len(other)
            a, b = self._pad(self.lines, other)
            newlines = [i+j for i,j in zip(a, b)]
            return self.__class__(newlines)
        elif type(other) == str:
            return self + self.__class__.center(other, len(self.lines))
        elif isinstance(other, self.__class__):
            return self + other.lines
        else:
            raise NotImplemented()
          
    def __radd__(self, other):
        if type(other) == list:
            return self.__class__(other) + self
        elif type(other) == str:
            return self.__class__.center(other, len(self.lines)) + self
        elif isinstance(other, self.__class__):
            return other + self
        else:
            raise NotImplemented()
        
    def __len__(self):
        return len(self.lines)
    
    def __str__(self):
        return "\n".join(self.lines)
    
def pretty(ex):
    return sp.printing.pretty(ex, use_unicode=False, wrap_line=False)

def make_cpp_vector(name, tp, values, maxw = 90):
    string = "std::vector<{type}> {name} = {{\n".format(type=tp, name=name)
    line = ""
    for idx, v in enumerate(values):
        if len(line)+len(v) > maxw:
            string += "\t" + line.strip() + "\n"
            line = ""
        line += v
        if idx < len(values)-1:
            line += ", "
    string += "\t" + line.strip()
    string += "\n};"
    return string
def printcpp(s):
    display(Markdown("```cpp\n%s\n```"%s))
    


def boundlim(obj, margin=0.2):
    xmin, ymin, xmax, ymax = obj.bounds
    return {
        "xlim": (xmin-margin, xmax+margin),
        "ylim": (ymin-margin, ymax+margin)
    }

def latex(s, *args):
    def convert(a):
        if type(a).__module__.startswith("sympy"):
            return sp.latex(a)
        return a
    nargs = tuple(map(convert, args))    
    display(Markdown(s % nargs))

