Import("env")

env.AddPostAction("${PROGPATH}", Action("cp -vt . $TARGET"))
