(TeX-add-style-hook "slides"
 (lambda ()
    (TeX-add-symbols
     '("btIfInRange" 2)
     "bt")
    (TeX-run-style-hooks
     "tikz"
     "siunitx"
     "fontspec"
     "hyperref"
     "lstlinebgrd"
     "dirtree"
     "listings"
     "pgffor"
     "pgf"
     "booktabs"
     "ragged2e"
     "tabularx"
     "multicol"
     "graphicx"
     "array"
     "pgfplots"
     ""
     "babel"
     "portuguese"
     "latex2e"
     "beamer10"
     "beamer"
     "compress")))

