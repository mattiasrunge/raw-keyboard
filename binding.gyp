{
    "targets": [
        {
            "target_name": "binding",
            "sources": [
                "src/keyboard.cc"
            ],
            "include_dirs" : [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}
