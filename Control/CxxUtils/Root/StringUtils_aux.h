/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StringUtils_aux.h,v 1.0 2014-05-25 16:54 cburgard Exp $
/**
 * @file  StringUtils_aux.h
 * @author carsten burgard <cburgarc@cern.ch>
 * @date May, 2014
 * @brief namespace for misc string utility
 *
 * This is an auxiliary header file that contains const-arrays used by
 * the StringUtils package. These definitions have been removed from
 * the StringUtils.cxx implementation file in order to improve
 * readability of the latter file.
 */

#ifndef CXXUTILS_STRINGUTILS_AUX_H
#define CXXUTILS_STRINGUTILS_AUX_H

// this array maps the first byte of a character in utf8 encoding to
// the total number of bytes that comprise this character
const size_t utf8_skip_data[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

// this array maps ascii symbols to the corresponding subscript and
// superscript unicode symbols. The script type is defined by the enum
// StringUtils::SPECIALSCRIPT in StringUtils.h
#define STRINGUTILS__N_MAP_SPECIALSCRIPT 70
const std::string map_specialscript[STRINGUTILS__N_MAP_SPECIALSCRIPT][3] = {
      {"0","⁰","0" }, 
      {"1","¹","₁"} ,
      {"2","²","₂"},
      {"3","³","₃"},
      {"4","⁴","₄"},
      {"5","⁵","₅"},
      {"6","⁶","₆"},
      {"7","⁷","₇"},
      {"8","⁸","₈"},
      {"9","⁹","₉"},
      {"+","⁺","₊"},
      {"-","⁻","₋"},
      {"=","⁼","₌"},
      {"(","⁽","₍"},
      {")","⁾","₎"},
      {"a","ᵃ","ₐ"},
      {"b","ᵇ","b"},
      {"c","ᶜ","c"},
      {"d","ᵈ","d"},
      {"e","ᵉ","ₑ"},
      {"f","ᶠ","f"},
      {"g","ᵍ","g"},
      {"h","ʰ","ₕ"},
      {"i","ⁱ","ᵢ"},
      {"j","ʲ","ⱼ"},
      {"k","ᵏ","ₖ"},
      {"l","ˡ","ₗ"},
      {"m","ᵐ","ₘ"},
      {"n","ⁿ","ₙ"},
      {"o","ᵒ","ₒ"},
      {"p","ᵖ","ₚ"},
      {"r","ʳ","ᵣ"},
      {"s","ˢ","ₛ"},
      {"t","ᵗ","ₜ"},
      {"u","ᵘ","ᵤ"},
      {"v","ᵛ","ᵥ"},
      {"w","ʷ","w"},
      {"x","ˣ","ₓ"},
      {"y","ʸ","y"},
      {"z","ᶻ","z"},
      {"A","ᴬ","A"},
      {"B","ᴮ","B"},
      {"D","ᴰ","D"},
      {"E","ᴱ","E"},
      {"G","ᴳ","G"},
      {"H","ᴴ","H"},
      {"I","ᴵ","I"},
      {"J","ᴶ","J"},
      {"K","ᴷ","K"},
      {"L","ᴸ","L"},
      {"M","ᴹ","M"},
      {"N","ᴺ","N"},
      {"O","ᴼ","O"},
      {"P","ᴾ","P"},
      {"R","ᴿ","R"},
      {"T","ᵀ","T"},
      {"U","ᵁ","U"},
      {"V","ⱽ","V"},
      {"W","ᵂ","W"},
      {"α","ᵅ","α"},
      {"β","ᵝ","ᵦ"},
      {"γ","ᵞ","ᵧ"},
      {"δ","ᵟ","δ"},
      {"ρ","ρ","ᵨ"},
      {"ε","ᵋ","ε"},
      {"θ","ᶿ","θ"},
      {"ι","ᶥ","ι"},
      {"Φ","ᶲ","Φ"},
      {"φ","ᵠ","ᵩ"},
      {"χ","ᵡ","ᵪ"}
    };
// these arrays contain the prefixes and suffixes for subscript and superscript in various formats
const std::string map_superscript_starters[4] = {
  "","","\\ensuremath{^{","<sup>"
};
const std::string map_superscript_enders[4] = {
  "","","}}","</sup>"
};
const std::string map_subscript_starters[4] = {
  "","","\\ensuremath{_{","<sub>"
};
const std::string map_subscript_enders[4] = {
  "","","}}","</sub>"
};



// this array maps special symbols in different formats to each
// other. the formats and their indices are defined by the enum
// StringUtils::FORMAT in StringUtils.h
// this list is far from complete and should be extended
// if need be.
#define STRINGUTILS__N_MAP_FORMAT 94
const std::string map_format[STRINGUTILS__N_MAP_FORMAT][4] = {
      { " "      ," "," "        ," "	          },
      { " "      ," ","~"        ," "	          },
      { "x"      ,"x", "x"        ,"x"	          },
      { "0"      ,"0", "0"        ,"0"	          },
      { "E"      ,"E", "E"        ,"E"	          },
      { "Alpha",  "Α", "\\Alpha",  "&Alpha;"      }, 	
      { "Beta",   "Β", "\\Beta",   "&Beta;"       }, 	
      { "Gamma",  "Γ", "\\Gamma",  "&Gamma;"      }, 	
      { "Delta",  "Δ", "\\Delta",  "&Delta;"      }, 	
      { "Epsilon","Ε", "\\Epsilon","&Epsilon;"    }, 	
      { "Zeta",   "Ζ", "\\Zeta",   "&Zeta;"       }, 
      { "Eta",    "Η", "\\Eta",    "&Eta;"        }, 
      { "Theta",  "Θ", "\\Theta",  "&Theta;"      }, 	
      { "Iota",   "I", "\\Iota",   "&Iota;"       }, 						
      { "Kappa",  "Κ", "\\Kappa",  "&Kappa;"      }, 
      { "Lambda", "Λ", "\\Lambda", "&Lambda;"     }, 	
      { "Mu",     "M", "\\Mu",     "&Mu;"         }, 
      { "Nu",     "Ν", "\\Nu",     "&Nu;"         },   
      { "Xi",     "Ξ", "\\Xi",     "&Xi;"         }, 
      { "Omicron","Ο", "\\Omicron","&Omicron;"    }, 	
      { "Pi",     "Π", "\\Pi",     "&Pi;"         }, 
      { "Rho",    "Ρ", "\\Rho",    "&Rho;"        }, 
      { "Sigma",  "Σ", "\\Sigma",  "&Sigma;"      }, 
      { "Tau",    "Τ", "\\Tau",    "&Tau;"        }, 
      { "Upsilon","Υ", "\\Upsilon","&Upsilon;"    }, 	
      { "Phi",    "Φ", "\\Phi",    "&Phi;"        }, 
      { "Chi",    "Χ", "\\Chi",    "&Chi;"        }, 
      { "Psi",    "Ψ", "\\Psi",    "&Psi;"        }, 
      { "Omega",  "Ω", "\\Omega",  "&Omega;"      }, 
      { "alpha"  ,"α", "\\alpha"  ,"&alpha;"      },
      { "beta"   ,"β", "\\beta"   ,"&beta;"       },
      { "gamma"  ,"γ", "\\gamma"  ,"&gamma;"      },
      { "delta"  ,"δ", "\\delta"  ,"&delta;"      },
      { "epsilon","ε", "\\epsilon","&epsilon;"    },
      { "zeta"   ,"ζ", "\\zeta"   ,"&zeta;"       },
      { "eta"    ,"η", "\\eta"    ,"&eta;"        },
      { "theta"  ,"θ", "\\theta"  ,"&theta;"      },
      { "iota"   ,"ι", "\\iota"   ,"&iota;"       },
      { "kappa"  ,"κ", "\\kappa"  ,"&kappa;"      },
      { "lambda" ,"λ", "\\lambda" ,"&lambda;"     },
      { "mu"     ,"μ", "\\mu"     ,"&mu;"         },
      { "nu"     ,"ν", "\\nu"     ,"&nu;"         },
      { "xi"     ,"ξ", "\\xi"     ,"&xi;"         },
      { "omicron","ο", "\\omicron","&omicron;"    },
      { "pi"     ,"π", "\\pi"     ,"&pi;"         },
      { "rho"    ,"ρ", "\\rho"    ,"&rho;"        },
      { "sigma"  ,"σ", "\\sigma"  ,"&sigma;"      },
      { "varsigma","ς", "\\varsigma","&vsigma;"   },
      { "tau"    ,"τ", "\\tau"    ,"&tau;"        },
      { "upsilon","υ", "\\upsilon","&upsilon;"    },
      { "phi"    ,"φ", "\\phi"    ,"&phi;"        },
      { "chi"    ,"χ", "\\chi"    ,"&chi;"        },
      { "psi"    ,"ψ", "\\psi"    ,"&psi;"        },
      { "omega"  ,"ω", "\\omega"  ,"&omega;"      },
      { "-|"     ,"¬", "\\neg"	  ,"&not;"        },  	
      { "+/-"    ,"±", "\\pm"	  ,"&plusmn;"     },
      { "*"      ,"*", "\\ast"	  ,"&ast;"        },
      { "*"      ,"·", "\\cdot"	  ,"&middot;"     },
      { "x"      ,"×", "\\times"  ,"&times;"      },	
      { "->"     ,"→", "\\righarrow" ,"&rarr"     },
      { "->"     ,"→", "\\to"        ,"&rarr"     },
      { "<-"     ,"←", "\\leftarrow" ,"&larr"     },
      { "<="     ,"⇐", "\\Leftarrow" ,"&lArr;"    },
      { "=>"     ,"⇒", "\\Rightarrow","&rArr;"    },
      { "<=>"    ,"⇔", "\\Leftrightarrow","&hArr;"},
      { "(Vx)"   ,"∀", "\\forall" ,"&forall;"     },
      { "del"    ,"∂", "\\partial","&part;"       },
      { "(Ex)"   ,"∃", "\\exists" ,"&exist;"      },
      { "0"      ,"∅","\\emptyset","&empty;"      },
      { "Nabla"  ,"∇","\\nabla"   ,"&nabla;"      },
      { "inf"    ,"∞","\\infty"   ,"&infin;"      },
      { "isElem" ,"∈","\\in"      ,"&isin;"       },
      { "is!Elem","∉","\\not\\in" ,"&notin;"      },
      { "Prod|"  ,"∏","\\prod"    ,"&prod;"       },
      { "Sum|"   ,"∑","\\sum"     ,"&sum;"        },
      { "sqrt|"  ,"√","\\surd"    ,"&radic;"      },
      { "/\\"    ,"∧","\\wedge"   ,"&and;"        },
      { "\\/"    ,"∨","\\vee"     ,"&or;"         },
      { "/\\"    ,"∩","\\cap"     ,"&cap;"        },
      { "\\/"    ,"∪","\\cup"     ,"&cup;"        },
      { "Int|"   ,"∫","\\int"     ,"&int;"        },
      { "~"      ,"≈","\\approx"  ,"&asymp;"      },
      { "!="     ,"≠","\\neq"     ,"&ne;"         },
      { "=="     ,"≡","\\equiv"   ,"&equiv;"      },
      { "<="     ,"≤","\\leq"     ,"&le;"         },
      { ">="     ,"≥","\\geq"     ,"&ge;"         },
      { ">"      ,"⊂","\\subset"  ,"&sub;"        },
      { "<"      ,"⊃","\\supset"  ,"&sup;"        },
      { "°"      ,"°","^\\circ"   ,"&deg;"        },
      { "floor[" ,"⌊","\\lfloor"  ,"&lfloor;"     },
      { "ceil["  ,"⌈","\\lceil"   ,"&lceil;"      },
      { "]"      ,"]","\\["       ,"["	          },
      { "]"      ,"⌋","\\rfloor"  ,"&rfloor;"     },
      { "]"      ,"⌉","\\rceil"   ,"&rceil;"      }
};

#endif //CXXUTILS_STRINGUTILS_AUX_H
