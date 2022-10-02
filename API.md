# TODO

What does this needs to do?
1. start an HTTP server in a thread -- only 1 connection a time is allowed.
2. stop the server and the thread when the program finishes
3. interpret certain "events":
- enter something (abc 123 ...)
- tap something
- swipe something (start - stop - speed)
- **** pinch something --> not going to do this, seems convoluted: https://stackoverflow.com/a/25629952/577669
- take a screenshot (cropped or not)

Events API expected:
enter_text|The strawberry shampoo smells better than it tastes.
tap|x1|y1
swipe|x1|y1|x2|y2|speed  # speed in milliseconds
screenshot|x1|y1|x2|y2   # the crop is optional

Events reply:
OK

in the case of a screenshot: the raw data, with the appropriate Content-Type header
