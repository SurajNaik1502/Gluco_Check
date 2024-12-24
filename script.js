function updateMeter(level) {
    const indicator = document.querySelector('.indicator');

    if (level === 'low') {
        indicator.style.transform = 'translateX(0%)';
    } else if (level === 'medium') {
        indicator.style.transform = 'translateX(100%)';
    } else if (level === 'high') {
        indicator.style.transform = 'translateX(200%)';
    }
}

setInterval(function() {
    fetch('http://127.0.0.1:5000/glucose')
        .then(response => response.json())
        .then(data => {
            var glucoseLevel = parseFloat(data.glucose_level);
            document.getElementById('output').innerText = "Your Glucose Levels are: " + glucoseLevel;

            // Update the meter based on glucose level
            if (glucoseLevel < 40) {
                updateMeter('low');
            } else if (glucoseLevel >= 40 && glucoseLevel <= 70) {
                updateMeter('medium');
            } else {
                updateMeter('high');
            }
        })
        .catch(err => {
            console.error('Error fetching glucose level:', err);
        });
}, 1000);  // Update every 1 second

