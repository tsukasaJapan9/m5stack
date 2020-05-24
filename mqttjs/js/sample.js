'use strict;'

window.onload = function () {
    xhr = new XMLHttpRequest();

function post(no) {
    xhr.open("POST", 'localhost:8000/command', true);
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function() {
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            // リクエスト成功時の処理
        }
    }
    xhr.send("foo=bar&lorem=ipsum");  
}


    document.getElementById('forward').onclick = () => {
        console.log("forward");
        post("1")
    };

    document.getElementById('backward').onclick = () => {
        console.log("bachward");
        post("2")
    };

    // xhr = new XMLHttpRequest();

    // xhr.onload = function (e) {
    //     if (xhr.readyState === 4) {
    //       if (xhr.status === 200) {
    //         var answer = document.getElementById('answer');
    //         answer.value = xhr.responseText;
    //       }
    //     }
    //   };
};



// function post(no) {
//     xhr.send("foo=bar&lorem=ipsum");
// } 


// function post(command_no) {
//     xhr.open('POST', 'localhost:8000/command', true);
//     xhr.setRequestHeader('content-type', 'application/x-www-form-urlencoded;charset=UTF-8');
//     data = encodeURIComponent('command') + '=' + encodeURIComponent(command_no)
//     xhr.send(data);
// }