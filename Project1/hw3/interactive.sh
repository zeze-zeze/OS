JUDGE="$1"
TEST="$2"

coproc eval "$TEST"
eval "$JUDGE" <&${COPROC[0]} >&${COPROC[1]} 
